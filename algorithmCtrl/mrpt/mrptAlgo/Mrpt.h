/***************************
@Author: Chunel
@Contact: chunel@foxmail.com
@File: Mrpt.h
@Time: 2020/12/4 7:45 下午
@Desc:
***************************/

#ifndef CAISS_MRPT_H
#define CAISS_MRPT_H


#include <algorithm>
#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Dense>
#include <Eigen/SparseCore>


struct Mrpt_Parameters {
    int n_trees = 0; /**< Number of trees in the label. */
    int depth = 0; /**< Depth of the trees in the label. */
    int k = 0; /**< Number of nearest neighbors searched for (if the label is autotuned; otherwise 0). */
    int votes = 0; /**< Optimal vote threshold (if the label is autotuned and the target recall is set; otherwise 0). */
    double estimated_qtime = 0.0; /**< Estimated query time (if the label is autotuned and the target recall is set; otherwise 0.0). */
    double estimated_recall = 0.0; /**< Estimated recall (if the label is autotuned and the target recall is set; otherwise 0.0). */
};

class Mrpt {
public:
    /** @name Constructors
    * The constructor does not actually build the label. The building is done
    * by the function grow() which has to be called before queries can be made.
    * There are two different versions of the constructor which differ only
    * by the type of the input data. The first version takes the data set
    * as `Ref` to `MatrixXf`, which means that the argument
    * can be either `MatrixXf` or `Map<MatrixXf>` (also certain blocks of `MatrixXf`
    * may be accepted, see [Eigen::Ref](https://eigen.tuxfamily.org/dox/TopicFunctionTakingEigenTypes.html)
    * for more information). The second version takes a float
    * pointer to an array containing the data set, and the dimension and
    * the sample size of the data. There are also corresponding versions
    * of all the member functions which take input data. In all cases the data
    * is assumed to be stored in column-major order such that each data point
    * is stored contiguously in memory. In all cases no copies are made of
    * the original data matrix. */

    /**
    * @param X_ Eigen ref to the data set, stored as one data point per column
    */
    Mrpt(const Eigen::Ref<const Eigen::MatrixXf> &X_) :
            X(Eigen::Map<const Eigen::MatrixXf>(X_.data(), X_.rows(), X_.cols())),
            n_samples(X_.cols()),
            dim(X_.rows()) {}

    /**
    * @param X_ a float array containing the data set with each data point
    * stored contiguously in memory
    * @param dim_ dimension of the data
    * @param n_samples_ number of data points
    */
    Mrpt(const float *X_, int dim_, int n_samples_) :
            X(Eigen::Map<const Eigen::MatrixXf>(X_, dim_, n_samples_)),
            n_samples(n_samples_),
            dim(dim_) {}


    /**@}*/

    /** @name Normal label building.
    * Build a normal (not autotuned) label.
    */

    /**
    * Build a normal label.
    *
    * @param n_trees_ number of trees to be grown
    * @param depth_ depth of the trees; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$ is the number
    * of data points
    * @param density_ expected proportion of non-zero components in the
    * random vectors; on the interval \f$(0,1]\f$; default value sets density to
    * \f$ 1 / \sqrt{d} \f$, where \f$d\f$ is the dimension of the data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    */
    void grow(int n_trees_, int depth_, float density_ = -1.0, int seed = 0) {

        if (!empty()) {
            throw std::logic_error("The label has already been grown.");
        }

        if (n_trees_ <= 0) {
            throw std::out_of_range("The number of trees must be positive.");
        }

        if (depth_ <= 0 || depth_ > std::log2(n_samples)) {
            throw std::out_of_range("The depth must belong to the set {1, ... , log2(n)}.");
        }

        if (density_ < -1.0001 || density_ > 1.0001 || (density_ > -0.9999 && density_ < -0.0001)) {
            throw std::out_of_range("The density must be on the interval (0,1].");
        }

        n_trees = n_trees_;
        depth = depth_;
        n_pool = n_trees_ * depth_;
        n_array = 1 << (depth_ + 1);

        if (density_ < 0) {
            density = 1.0 / std::sqrt(dim);
        } else {
            density = density_;
        }

        density < 1 ? build_sparse_random_matrix(sparse_random_matrix, n_pool, dim, density, seed) :
        build_dense_random_matrix(dense_random_matrix, n_pool, dim, seed);

        split_points = Eigen::MatrixXf(n_array, n_trees);
        tree_leaves = std::vector<std::vector<int>>(n_trees);

        count_first_leaf_indices_all(leaf_first_indices_all, n_samples, depth);
        leaf_first_indices = leaf_first_indices_all[depth];

#pragma omp parallel for
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            Eigen::MatrixXf tree_projections;

            if (density < 1)
                tree_projections.noalias() = sparse_random_matrix.middleRows(n_tree * depth, depth) * X;
            else
                tree_projections.noalias() = dense_random_matrix.middleRows(n_tree * depth, depth) * X;

            tree_leaves[n_tree] = std::vector<int>(n_samples);
            std::vector<int> &indices = tree_leaves[n_tree];
            std::iota(indices.begin(), indices.end(), 0);

            grow_subtree(indices.begin(), indices.end(), 0, 0, n_tree, tree_projections);
        }
    }

    /**@}*/

    /** @name Autotuned label building
    * Builds an label by autotuning such that the parameters giving the fastest
    * query time at the target recall level are found. If the target recall level
    * is not reached at all, then an label giving the highest recall level
    * is built. The parameters() function can be used to retrieve these optimal
    * parameter values and the estimated query time and the estimated recall.
    * There is a version which uses a separate set of test queries (`grow`),
    * and a version which samples a test set from the data set (`grow_autotune`).
    */

    /**
    * Build an autotuned label.
    *
    * @param target_recall target recall level; on the range [0,1]
    * @param Q Eigen ref to the the test queries (col = data point, row = dimension).
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max maximum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density expected proportion of non-zero components in the random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    */
    void grow(double target_recall, const Eigen::Ref<const Eigen::MatrixXf> &Q, int k_, int trees_max = -1,
              int depth_max = -1, int depth_min_ = -1, int votes_max_ = -1,
              float density = -1.0, int seed = 0) {
        if (target_recall < 0.0 - epsilon || target_recall > 1.0 + epsilon) {
            throw std::out_of_range("Target recall must be on the interval [0,1].");
        }

        grow(Q, k_, trees_max, depth_max, depth_min_, votes_max_, density, seed);
        prune(target_recall);
    }

    /** Build an autotuned label.
    *
    * @param target_recall target recall level; on the range [0,1]
    * @param Q float array containing the test queries
    * @param n_test number of test queries
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max maximum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density expected proportion of non-zero components in the random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    * @param indices_test parameter used by the version which uses no
    * separate test set, leave empty.
    */
    void grow(double target_recall, const float *Q, int n_test, int k_, int trees_max = -1,
              int depth_max = -1, int depth_min_ = -1, int votes_max_ = -1,
              float density = -1.0, int seed = 0, const std::vector<int> &indices_test = {}) {
        if (target_recall < 0.0 - epsilon || target_recall > 1.0 + epsilon) {
            throw std::out_of_range("Target recall must be on the interval [0,1].");
        }

        grow(Q, n_test, k_, trees_max, depth_max, depth_min_, votes_max_, density, seed, indices_test);
        prune(target_recall);
    }

    /** Build an autotuned label sampling test queries from the training set.
    *
    * @param target_recall target recall level; on the range [0,1]
    * @param n_test number of test queries
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max maximum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density_ expected proportion of non-zero components in the random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    * @param n_test number of test queries sampled from the training set.
    */
    void grow_autotune(double target_recall, int k_, int trees_max = -1, int depth_max = -1, int depth_min_ = -1,
                       int votes_max_ = -1, float density_ = -1.0, int seed = 0, int n_test = 100) {
        if (n_test < 1) {
            throw std::out_of_range("Test set size must be > 0.");
        }

        n_test = n_test > n_samples ? n_samples : n_test;
        std::vector<int> indices_test(sample_indices(n_test, seed));
        const Eigen::MatrixXf Q(subset(indices_test));

        grow(target_recall, Q.data(), Q.cols(), k_, trees_max,
             depth_max, depth_min_, votes_max_, density_, seed, indices_test);
    }

    /**
    * Get the optimal parameters and the estimated recall and query time found
    * by autotuning. If the label is autotuned without preset recall level,
    * `estimated_recall`, `estimated_qtime` and `votes` are set to their
    * default value 0, and `n_trees` and `depth` are set to `trees_max` and
    * `depth_max, respectively. If the label is not autotuned,
    * `estimated_recall`, `estimated_qtime`, `votes` and `k` are all set to
    * their default value 0.
    *
    * @return parameters of the label
    */
    Mrpt_Parameters parameters() const {
        if (index_type == normal || index_type == autotuned_unpruned) {
            Mrpt_Parameters p;
            p.n_trees = n_trees;
            p.depth = depth;
            p.k = par.k;
            return p;
        }

        return par;
    }

    /**
     * Get whether the label has been autotuned.
     *
     * @return true if the label has been autotuned, false otherwise.
    */
    bool is_autotuned() const {
        return index_type == autotuned;
    }

    /**@}*/

    /** @name Autotuned label building without preset recall level
    * Build an autotuned label. This version does not require prespecifying
    * a target recall level, but an label generated by this function can be used
    * to subset different indices with different recall levels. This is done by
    * subset(). The function optimal_parameters() can be used to retrieve a
    * pareto frontier of optimal parameters. There is a version which uses a
    * separate set of test queries (`grow`), and a version which samples a
    * test set from the data set (`grow_autotune`).
    */

    /**@{*/

    /** Build an autotuned label without prespecifying a recall level.
    *
    * @param data a float array containing the test queries.
    * @param n_test number of test queries
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max maximum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density_ expected proportion of non-zero components in the random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    * @param indices_test parameter used by the version which uses no
    * separate test set, leave empty.
    **/
    void grow(const float *data, int n_test, int k_, int trees_max = -1, int depth_max = -1,
              int depth_min_ = -1, int votes_max_ = -1, float density_ = -1.0, int seed = 0,
              const std::vector<int> &indices_test = {}) {

        if (trees_max == -1) {
            trees_max = std::min(std::sqrt(n_samples), 1000.0);
        }

        if (depth_min_ == -1) {
            depth_min_ = std::max(static_cast<int>(std::log2(n_samples) - 11), 5);
        }

        if (depth_max == -1) {
            depth_max = std::max(static_cast<int>(std::log2(n_samples) - 4), depth_min_);
        }

        if (votes_max_ == -1) {
            votes_max_ = std::max(trees_max / 10, std::min(trees_max, 10));
        }

        if (density_ > -1.0001 && density_ < -0.9999) {
            density_ = 1.0 / std::sqrt(dim);
        }

        if (!empty()) {
            throw std::logic_error("The label has already been grown.");
        }

        if (k_ <= 0 || k_ > n_samples) {
            throw std::out_of_range("k_ must belong to the set {1, ..., n}.");
        }

        if (trees_max <= 0) {
            throw std::out_of_range("trees_max must be positive.");
        }

        if (depth_max <= 0 || depth_max > std::log2(n_samples)) {
            throw std::out_of_range("depth_max must belong to the set {1, ... , log2(n)}.");
        }

        if (depth_min_ <= 0 || depth_min_ > depth_max) {
            throw std::out_of_range("depth_min_ must belong to the set {1, ... , depth_max}");
        }

        if (votes_max_ <= 0 || votes_max_ > trees_max) {
            throw std::out_of_range("votes_max_ must belong to the set {1, ... , trees_max}.");
        }

        if (density_ < 0.0 || density_ > 1.0001) {
            throw std::out_of_range("The density must be on the interval (0,1].");
        }

        if (n_samples < 101) {
            throw std::out_of_range("Sample size must be at least 101 to autotune an label.");
        }

        depth_min = depth_min_;
        votes_max = votes_max_;
        k = k_;

        const Eigen::Map<const Eigen::MatrixXf> Q(data, dim, n_test);

        grow(trees_max, depth_max, density_, seed);
        Eigen::MatrixXi exact(k, n_test);
        compute_exact(Q, exact, indices_test);

        std::vector<Eigen::MatrixXd> recalls(depth_max - depth_min + 1);
        cs_sizes = std::vector<Eigen::MatrixXd>(depth_max - depth_min + 1);

        for (int d = depth_min; d <= depth_max; ++d) {
            recalls[d - depth_min] = Eigen::MatrixXd::Zero(votes_max, trees_max);
            cs_sizes[d - depth_min] = Eigen::MatrixXd::Zero(votes_max, trees_max);
        }

        for (int i = 0; i < n_test; ++i) {
            std::vector<Eigen::MatrixXd> recall_tmp(depth_max - depth_min + 1);
            std::vector<Eigen::MatrixXd> cs_size_tmp(depth_max - depth_min + 1);

            count_elected(Q.col(i), Eigen::Map<Eigen::VectorXi>(exact.data() + i * k, k),
                          votes_max, recall_tmp, cs_size_tmp);

            for (int d = depth_min; d <= depth_max; ++d) {
                recalls[d - depth_min] += recall_tmp[d - depth_min];
                cs_sizes[d - depth_min] += cs_size_tmp[d - depth_min];
            }
        }

        for (int d = depth_min; d <= depth_max; ++d) {
            recalls[d - depth_min] /= (k * n_test);
            cs_sizes[d - depth_min] /= n_test;
        }

        fit_times(Q);
        std::set<Mrpt_Parameters, decltype(is_faster) *> pars = list_parameters(recalls);
        opt_pars = pareto_frontier(pars);

        index_type = autotuned_unpruned;
        par.k = k_;
    }

    /** Build an autotuned label without prespecifying a recall level.
    *
    * @param Q Eigen ref to the test queries.
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max depth of trees grown; ; on the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters on the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density_ expected proportion of non-zero components of random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    */
    void grow(const Eigen::Ref<const Eigen::MatrixXf> &Q, int k_, int trees_max = -1, int depth_max = -1,
              int depth_min_ = -1, int votes_max_ = -1, float density_ = -1.0, int seed = 0) {
        if (Q.rows() != dim) {
            throw std::invalid_argument("Dimensions of the data and the validation set do not match.");
        }

        grow(Q.data(), Q.cols(), k_, trees_max,
             depth_max, depth_min_, votes_max_, density_, seed);
    }

    /** Build an autotuned label sampling test queries from the training set
    * and without prespecifying a recall level.
    *
    * @param k_ number of nearest neighbors searched for
    * @param trees_max number of trees grown; default value -1 sets this to
    * \f$ \mathrm{min}(\sqrt{n}, 1000)\f$, where \f$n\f$ is the number of data points.
    * @param depth_max depth of trees grown; in the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$, where \f$n \f$
    * is the number of data points; default value -1 sets this to
    * \f$ \log_2(n) - 4 \f$, where \f$n\f$ is the number of data points
    * @param depth_min_ minimum depth of trees considered when searching for
    * optimal parameters on the set
    * \f$\{1,2, \dots ,\lfloor \log_2 (n) \rfloor \}\f$; a default value -1
    * sets this to \f$ \mathrm{max}(\lfloor \log_2 (n) \rfloor - 11, 5)\f$
    * @param votes_max_ maximum number of votes considered when searching for
    * optimal parameters; a default value -1 sets this to
    * \f$ \mathrm{max}(\lfloor \mathrm{trees\_max} / 10 \rfloor,
    * \mathrm{min}(10, \mathrm{trees\_max})) \f$
    * @param density_ expected proportion of non-zero components of random vectors;
    * default value -1.0 sets this to \f$ 1 / \sqrt{d} \f$, where \f$ d\f$ is
    * the dimension of data
    * @param seed seed given to a rng when generating random vectors;
    * a default value 0 initializes the rng randomly with std::random_device
    * @param n_test number of test queries sampled from the training set.
    */
    void grow_autotune(int k_, int trees_max = -1, int depth_max = -1, int depth_min_ = -1,
                       int votes_max_ = -1, float density_ = -1.0, int seed = 0, int n_test = 100) {
        if (n_test < 1) {
            throw std::out_of_range("Test set size must be > 0.");
        }

        n_test = n_test > n_samples ? n_samples : n_test;
        std::vector<int> indices_test(sample_indices(n_test, seed));
        const Eigen::MatrixXf Q(subset(indices_test));

        grow(Q.data(), Q.cols(), k_, trees_max,
             depth_max, depth_min_, votes_max_, density_, seed, indices_test);
    }

    /** Create a new label by copying trees from an autotuned label grown
    * without a prespecified recall level. The label is created so that
    * it gives a fastest query time at the recall level given as the parameter.
    * If this recall level is not met, then it creates an label with a
    * highest possible recall level.
    *
    * @param target_recall target recall level; on the range [0,1]
    * @return an autotuned Mrpt label with a recall level at least as high as
    * target_recall
    */
    Mrpt subset(double target_recall) const {
        if (target_recall < 0.0 - epsilon || target_recall > 1.0 + epsilon) {
            throw std::out_of_range("Target recall must be on the interval [0,1].");
        }

        Mrpt index2(X);
        index2.par = parameters(target_recall);

        int depth_max = depth;

        index2.n_trees = index2.par.n_trees;
        index2.depth = index2.par.depth;
        index2.votes = index2.par.votes;
        index2.n_pool = index2.depth * index2.n_trees;
        index2.n_array = 1 << (index2.depth + 1);
        index2.tree_leaves.assign(tree_leaves.begin(), tree_leaves.begin() + index2.n_trees);
        index2.leaf_first_indices_all = leaf_first_indices_all;
        index2.density = density;
        index2.k = k;

        index2.split_points = split_points.topLeftCorner(index2.n_array, index2.n_trees);
        index2.leaf_first_indices = leaf_first_indices_all[index2.depth];
        if (index2.density < 1) {
            index2.sparse_random_matrix = Eigen::SparseMatrix<float, Eigen::RowMajor>(index2.n_pool, index2.dim);
            for (int n_tree = 0; n_tree < index2.n_trees; ++n_tree)
                index2.sparse_random_matrix.middleRows(n_tree * index2.depth, index2.depth) =
                        sparse_random_matrix.middleRows(n_tree * depth_max, index2.depth);
        } else {
            index2.dense_random_matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>(
                    index2.n_pool, index2.dim);
            for (int n_tree = 0; n_tree < index2.n_trees; ++n_tree)
                index2.dense_random_matrix.middleRows(n_tree * index2.depth, index2.depth) =
                        dense_random_matrix.middleRows(n_tree * depth_max, index2.depth);
        }
        index2.index_type = autotuned;

        return index2;
    }


    /** Create a new label by copying trees from an autotuned label grown
    * without a prespecified recall level. The label is created so that
    * it gives a fastest query time at the recall level given as the parameter.
    * If this recall level is not met, then it creates an label with a
    * highest possible recall level. This function differs from subset() only
    * by the return value.
    *
    * @param target_recall target recall level; on the range [0,1]
    * @return pointer to a dynamically allocated autotuned Mrpt label with
    * a recall level at least as high as target_recall
    */
    Mrpt *subset_pointer(double target_recall) const {
        if (target_recall < 0.0 - epsilon || target_recall > 1.0 + epsilon) {
            throw std::out_of_range("Target recall must be on the interval [0,1].");
        }

        Mrpt *index2 = new Mrpt(X);
        index2->par = parameters(target_recall);

        int depth_max = depth;

        index2->n_trees = index2->par.n_trees;
        index2->depth = index2->par.depth;
        index2->votes = index2->par.votes;
        index2->n_pool = index2->depth * index2->n_trees;
        index2->n_array = 1 << (index2->depth + 1);
        index2->tree_leaves.assign(tree_leaves.begin(), tree_leaves.begin() + index2->n_trees);
        index2->leaf_first_indices_all = leaf_first_indices_all;
        index2->density = density;
        index2->k = k;

        index2->split_points = split_points.topLeftCorner(index2->n_array, index2->n_trees);
        index2->leaf_first_indices = leaf_first_indices_all[index2->depth];
        if (index2->density < 1) {
            index2->sparse_random_matrix = Eigen::SparseMatrix<float, Eigen::RowMajor>(index2->n_pool, index2->dim);
            for (int n_tree = 0; n_tree < index2->n_trees; ++n_tree)
                index2->sparse_random_matrix.middleRows(n_tree * index2->depth, index2->depth) =
                        sparse_random_matrix.middleRows(n_tree * depth_max, index2->depth);
        } else {
            index2->dense_random_matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>(
                    index2->n_pool, index2->dim);
            for (int n_tree = 0; n_tree < index2->n_trees; ++n_tree)
                index2->dense_random_matrix.middleRows(n_tree * index2->depth, index2->depth) =
                        dense_random_matrix.middleRows(n_tree * depth_max, index2->depth);
        }
        index2->index_type = autotuned;

        return index2;
    }


    /**
    * Return the pareto frontier of optimal parameters for an label which
    * is autotuned without setting a recall level. This means that each
    * parameter combination in a returned vector is optimal in a sense
    * that it is a fastest (measured by query time) parameter combination
    * to obtain as least as high recall level that it has.
    *
    * @return vector of optimal parameters
    */
    std::vector<Mrpt_Parameters> optimal_parameters() const {
        if (index_type == normal) {
            throw std::logic_error("The list of optimal parameters cannot be retrieved for the non-autotuned label.");
        }
        if (index_type == autotuned) {
            throw std::logic_error(
                    "The list of optimal parameters cannot be retrieved for the label which has already been subsetted or deleted to the target recall level.");
        }

        std::vector<Mrpt_Parameters> new_pars;
        std::copy(opt_pars.begin(), opt_pars.end(), std::back_inserter(new_pars));
        return new_pars;
    }

    /**@}*/

    /** @name Approximate k-nn search
    * A query using a non-autotuned label. Finds k approximate nearest neighbors
    * from a data set X for a query point q. Because the label is not autotuned,
    * k and vote threshold are set manually. The indices of k nearest neighbors
    * are written to a buffer out, which has to be preallocated to have at least
    * length k. Optionally also Euclidean distances to these k nearest points
    * are written to a buffer out_distances. If there are less than k points in
    * the candidate set, -1 is written to the remaining locations of the
    * output buffers.
    */

    /**
    * Approximate k-nn search using a normal label.
    *
    * @param data pointer to an array containing the query point
    * @param k number of nearest neighbors searched for
    * @param vote_threshold - number of votes required for a query point to be included in the candidate set
    * @param out output buffer (size = k) for the indices of k approximate nearest neighbors
    * @param out_distances optional output buffer (size = k) for distances to k approximate nearest neighbors
    * @param out_n_elected optional output parameter (size = 1) for the candidate set size
    */
    int query(const float *data, int k, int vote_threshold, int *out,
               float *out_distances = nullptr, int *out_n_elected = nullptr) const {
        if (k <= 0 || k > n_samples) {
            //throw std::out_of_range("k must belong to the set {1, ..., n}.");
            return -6;   // 表示参数问题，topK传递的值不对
        }

        if (vote_threshold <= 0 || vote_threshold > n_trees) {
            vote_threshold = (int)votes;    // 用建议votes值赋值
        }

        if (empty()) {
            return -12;    // 表示是模型问题
        }

        const Eigen::Map<const Eigen::VectorXf> q(data, dim);

        Eigen::VectorXf projected_query(n_pool);
        if (density < 1) {
            projected_query.noalias() = sparse_random_matrix * q;
        } else {
            projected_query.noalias() = dense_random_matrix * q;
        }

        std::vector<int> found_leaves(n_trees);

        /*
        * The following loops over all trees, and routes the query to exactly one
        * leaf in each.
        */
#pragma omp parallel for
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            int idx_tree = 0;
            for (int d = 0; d < depth; ++d) {
                const int j = n_tree * depth + d;
                const int idx_left = 2 * idx_tree + 1;
                const int idx_right = idx_left + 1;
                const float split_point = split_points(idx_tree, n_tree);
                if (projected_query(j) <= split_point) {
                    idx_tree = idx_left;
                } else {
                    idx_tree = idx_right;
                }
            }
            found_leaves[n_tree] = idx_tree - (1 << depth) + 1;
        }

        int n_elected = 0, max_leaf_size = n_samples / (1 << depth) + 1;
        Eigen::VectorXi elected(n_trees * max_leaf_size);    // 初始化的时候，这两个向量都是0*n向量
        Eigen::VectorXi votes = Eigen::VectorXi::Zero(n_samples);

        // count votes
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            int leaf_begin = leaf_first_indices[found_leaves[n_tree]];
            int leaf_end = leaf_first_indices[found_leaves[n_tree] + 1];
            const std::vector<int> &indices = tree_leaves[n_tree];
            for (int i = leaf_begin; i < leaf_end; ++i) {
                int idx = indices[i];
                // 每次votes(idx)的值都会+1，匹配到threshold，即返回
                // 所以，threshold值设置的越大，越难以匹配到，从而进入elected就少，从而导致准确度降低
                // 但计算的值少了，耗时就降低了
                if (++votes(idx) == vote_threshold)
                    elected(n_elected++) = idx;
            }
        }

        if (out_n_elected) {
            *out_n_elected = n_elected;    // 记录被粗选中的个数
        }

        exact_knn(q, k, elected, n_elected, out, out_distances);
        return 0;
    }

    /**
    *  Approximate k-nn search using a normal label.
    *
    * @param q Eigen ref to the query point
    * @param k number of nearest neighbors searched for
    * @param vote_threshold number of votes required for a query point to be included in the candidate set
    * @param out output buffer (size = k) for the indices of k approximate nearest neighbors
    * @param out_distances optional output buffer (size = k) for distances to k approximate nearest neighbors
    * @param out_n_elected optional output parameter (size = 1) for the candidate set size
    */
    void query(const Eigen::Ref<const Eigen::VectorXf> &q, int k, int vote_threshold, int *out,
               float *out_distances = nullptr, int *out_n_elected = nullptr) const {
        query(q.data(), k, vote_threshold, out, out_distances, out_n_elected);
    }

    /**@}*/

    /** @name Approximate k-nn search using autotuned label
    * Approximate k-nn search using an autotuned label. Finds k approximate
    * nearest neighbors from a data set X for a query point q. Because the label
    * is autotuned, no parameters other than a query point and an output are
    * required: k is preset, and the optimal vote count is used automatically.
    * The indices of k nearest neighbors are written to a buffer out, which has
    * to be preallocated to have at least length k. Optionally also the Euclidean
    * distances to these k nearest points are written to a buffer
    * out_distances. If there are less than k points in the candidate set,
    * -1 is written to the remaining locations of the output buffers.
    */

    /**
    * Approximate k-nn search using an autotuned label.
    *
    * @param q pointer to an array containing the query point
    * @param out output buffer (size = k) for the indices of k approximate nearest neighbors
    * @param out_distances optional output buffer (size = k) for distances to k approximate nearest neighbors
    * @param out_n_elected optional output parameter (size = 1) for the candidate set size
    */
    void query(const float *q, int *out, float *out_distances = nullptr,
               int *out_n_elected = nullptr) const {
        if (index_type == normal) {
            throw std::logic_error("The label is not autotuned: k and vote threshold has to be specified.");
        }

        if (index_type == autotuned_unpruned) {
            throw std::logic_error("The target recall level has to be set before making queries.");
        }

        query(q, k, votes, out, out_distances, out_n_elected);
    }

    /**
    * Approximate k-nn search using an autotuned label.
    *
    * @param q Eigen ref to the query point
    * @param out output buffer (size = k) for the indices of k approximate nearest neighbors
    * @param out_distances optional output buffer (size = k) for distances to k approximate nearest neighbors
    * @param out_n_elected optional output parameter (size = 1) for the candidate set size
    */
    void query(const Eigen::Ref<const Eigen::VectorXf> &q, int *out, float *out_distances = nullptr,
               int *out_n_elected = nullptr) const {
        query(q.data(), out, out_distances, out_n_elected);
    }


    /** @name Exact k-nn search
    * Functions for fast exact k-nn search: find k nearest neighbors for a
    * query point q from a data set X_. The indices of k nearest neighbors are
    * written to a buffer out, which has to be preallocated to have at least
    * length k. Optionally also the Euclidean distances to these k nearest points
    * are written to a buffer out_distances. There are both static and member
    * versions.
    */

    /**
    * @param q_data pointer to an array containing the query point
    * @param X_data pointer to an array containing the data set
    * @param dim dimension of data
    * @param n_samples number of points in a data set
    * @param k number of neighbors searched for
    * @param out output buffer (size = k) for the indices of k nearest neighbors
    * @param out_distances optional output buffer (size = k) for the distances to k nearest neighbors
    */
    static void exact_knn(const float *q_data, const float *X_data, int dim, int n_samples,
                          int k, int *out, float *out_distances = nullptr) {

        const Eigen::Map<const Eigen::MatrixXf> X(X_data, dim, n_samples);
        const Eigen::Map<const Eigen::VectorXf> q(q_data, dim);

        if (k < 1 || k > n_samples) {
            throw std::out_of_range("k must be positive and no greater than the sample size of data X.");
        }

        Eigen::VectorXf distances(n_samples);

#pragma omp parallel for
        for (int i = 0; i < n_samples; ++i)
            distances(i) = (X.col(i) - q).squaredNorm();

        if (k == 1) {
            Eigen::MatrixXf::Index index;
            distances.minCoeff(&index);
            out[0] = index;

            if (out_distances)
                out_distances[0] = std::sqrt(distances(index));

            return;
        }

        Eigen::VectorXi idx(n_samples);
        std::iota(idx.data(), idx.data() + n_samples, 0);
        std::partial_sort(idx.data(), idx.data() + k, idx.data() + n_samples,
                          [&distances](int i1, int i2) { return distances(i1) < distances(i2); });

        for (int i = 0; i < k; ++i)
            out[i] = idx(i);

        if (out_distances) {
            for (int i = 0; i < k; ++i)
                out_distances[i] = std::sqrt(distances(idx(i)));
        }
    }

    /**
    * @param q Eigen ref to a query point
    * @param X Eigen ref to a data set
    * @param k number of neighbors searched for
    * @param out output buffer (size = k) for the indices of k nearest neighbors
    * @param out_distances optional output buffer (size = k) for the distances to k nearest neighbors
    */
    static void exact_knn(const Eigen::Ref<const Eigen::VectorXf> &q,
                          const Eigen::Ref<const Eigen::MatrixXf> &X,
                          int k, int *out, float *out_distances = nullptr) {
        Mrpt::exact_knn(q.data(), X.data(), X.rows(), X.cols(), k, out, out_distances);
    }

    /**
    * @param q pointer to an array containing the query point
    * @param k number of neighbors searched for
    * @param out output buffer (size = k) for the indices of k nearest neighbors
    * @param out_distances optional output buffer (size = k) for the distances to k nearest neighbors
    */
    void exact_knn(const float *q, int k, int *out, float *out_distances = nullptr) const {
        Mrpt::exact_knn(q, X.data(), dim, n_samples, k, out, out_distances);
    }

    /**
    * @param q pointer to an array containing the query point
    * @param k number of points searched for
    * @param out output buffer (size = k) for the indices of k nearest neighbors
    * @param out_distances optional output buffer (size = k) for the distances to k nearest neighbors
    */
    void exact_knn(const Eigen::Ref<const Eigen::VectorXf> &q, int k, int *out,
                   float *out_distances = nullptr) const {
        Mrpt::exact_knn(q.data(), X.data(), dim, n_samples, k, out, out_distances);
    }

    /**@}*/

    /** @name Utility functions
    * Saving and loading an label and checking if it is already constructed.
    * Saving and loading work for both autotuned and non-autotuned indices, and
    * load() retrieves also the optimal parameters found by autotuning.
    * The same data set used to build a saved label has to be used to
    * construct the label into which it is loaded.
    */

    /**
    * Saves the label to a file.
    *
    * @param path - filepath to the output file.
    * @return true if saving succeeded, false otherwise.
    */
    bool save(const char *path, unsigned int max_data_size, int max_index_size,
              std::vector<std::string> &words) const {
        FILE *fd = nullptr;
        if ((fd = fopen(path, "wb")) == nullptr) {
            return false;
        }

        // 记录数据相关信息，供 Load 时候使用
        // fwrite 说明 ： (ptr, per-size, num, fd)
        fwrite(&MRPT_MODEL_TAG, sizeof(MRPT_MODEL_TAG), 1, fd);
        fwrite(&n_samples, sizeof(int), 1, fd);
        fwrite(&dim, sizeof(int), 1, fd);
        fwrite(&max_index_size, sizeof(unsigned int), 1, fd);
        int cur_size = (int)words.size();
        fwrite(&cur_size, sizeof(int), 1, fd);

        fwrite(X.data(), sizeof(float), dim*cur_size, fd);
        char* buf = new char[max_data_size];
        for (const auto &x : words) {
            memset(buf, 0, max_index_size);
            memcpy(buf, x.c_str(), x.size());
            fwrite(x.c_str(), 1, max_index_size, fd);
        }
        delete []buf;

        // 开始读取索引数据
        int i = index_type;
        fwrite(&i, sizeof(int), 1, fd);

        if (index_type == 2) {
            write_parameter_list(opt_pars, fd);
        }

        write_parameters(&par, fd);
        fwrite(&n_trees, sizeof(int), 1, fd);
        fwrite(&depth, sizeof(int), 1, fd);
        fwrite(&density, sizeof(float), 1, fd);

        fwrite(split_points.data(), sizeof(float), n_array * n_trees, fd);

        // save tree leaves
        for (int i = 0; i < n_trees; ++i) {
            int sz = tree_leaves[i].size();
            fwrite(&sz, sizeof(int), 1, fd);
            fwrite(&tree_leaves[i][0], sizeof(int), sz, fd);
        }

        // save random matrix
        if (density < 1) {
            int non_zeros = sparse_random_matrix.nonZeros();
            fwrite(&non_zeros, sizeof(int), 1, fd);
            for (int k = 0; k < sparse_random_matrix.outerSize(); ++k) {
                for (Eigen::SparseMatrix<float, Eigen::RowMajor>::InnerIterator it(sparse_random_matrix, k); it; ++it) {
                    float val = it.value();
                    int row = it.row(), col = it.col();
                    fwrite(&row, sizeof(int), 1, fd);
                    fwrite(&col, sizeof(int), 1, fd);
                    fwrite(&val, sizeof(float), 1, fd);
                }
            }
        } else {
            fwrite(dense_random_matrix.data(), sizeof(float), n_pool * dim, fd);
        }

        fclose(fd);
        return true;
    }

    /**
    * Loads an label from a file.
    *
    * @param path filepath to the label file.
    * @return true if loading succeeded, false otherwise.
    */
    bool load(const char *path, long skip_size) {
        FILE *fd = nullptr;
        if ((fd = fopen(path, "rb")) == nullptr)
            return false;

        // 先向后移动偏移量个信息
        fseek(fd, (long)skip_size, SEEK_SET);

        int i = 0;
        fread(&i, sizeof(int), 1, fd);
        index_type = static_cast<itype>(i);
        if (index_type == autotuned_unpruned) {
            read_parameter_list(fd);
        }

        read_parameters(&par, fd);
        fread(&n_trees, sizeof(int), 1, fd);
        fread(&depth, sizeof(int), 1, fd);
        fread(&density, sizeof(float), 1, fd);

        n_pool = n_trees * depth;
        n_array = 1 << (depth + 1);

        count_first_leaf_indices_all(leaf_first_indices_all, n_samples, depth);
        leaf_first_indices = leaf_first_indices_all[depth];

        split_points = Eigen::MatrixXf(n_array, n_trees);
        fread(split_points.data(), sizeof(float), n_array * n_trees, fd);

        // load tree leaves
        tree_leaves = std::vector<std::vector<int>>(n_trees);
        for (int i = 0; i < n_trees; ++i) {
            int sz;
            fread(&sz, sizeof(int), 1, fd);
            std::vector<int> leaves(sz);
            fread(&leaves[0], sizeof(int), sz, fd);
            tree_leaves[i] = leaves;
        }

        // load random matrix
        if (density < 1) {
            int non_zeros;
            fread(&non_zeros, sizeof(int), 1, fd);

            sparse_random_matrix = Eigen::SparseMatrix<float>(n_pool, dim);
            std::vector<Eigen::Triplet<float>> triplets;
            for (int k = 0; k < non_zeros; ++k) {
                int row, col;
                float val;
                fread(&row, sizeof(int), 1, fd);
                fread(&col, sizeof(int), 1, fd);
                fread(&val, sizeof(float), 1, fd);
                triplets.push_back(Eigen::Triplet<float>(row, col, val));
            }

            sparse_random_matrix.setFromTriplets(triplets.begin(), triplets.end());
            sparse_random_matrix.makeCompressed();
        } else {
            dense_random_matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>(n_pool, dim);
            fread(dense_random_matrix.data(), sizeof(float), n_pool * dim, fd);
        }

        fclose(fd);

        k = par.k;
        votes = par.votes;
        return true;
    }


    /**
    * Is the label is already constructed or not?
    *
    * @return - is the label empty?
    */
    bool empty() const {
        return n_trees == 0;
    }

    /**@}*/

    /** @name
    * Friend declarations for test fixtures. Tests are located at
    * https://github.com/vioshyvo/RP-test.
    */
    friend class MrptTest;

    friend class UtilityTest;

    /**@}*/


private:

    /**
    * Builds a single random projection tree. The tree is constructed by recursively
    * projecting the data on a random vector and splitting into two by the median.
    */
    void grow_subtree(std::vector<int>::iterator begin, std::vector<int>::iterator end,
                      int tree_level, int i, int n_tree, const Eigen::MatrixXf &tree_projections) {
        int n = end - begin;
        int idx_left = 2 * i + 1;
        int idx_right = idx_left + 1;

        if (tree_level == depth) return;

        std::nth_element(begin, begin + n / 2, end,
                         [&tree_projections, tree_level](int i1, int i2) {
                             return tree_projections(tree_level, i1) < tree_projections(tree_level, i2);
                         });
        auto mid = end - n / 2;

        if (n % 2) {
            split_points(i, n_tree) = tree_projections(tree_level, *(mid - 1));
        } else {
            auto left_it = std::max_element(begin, mid,
                                            [&tree_projections, tree_level](int i1, int i2) {
                                                return tree_projections(tree_level, i1) <
                                                       tree_projections(tree_level, i2);
                                            });
            split_points(i, n_tree) = (tree_projections(tree_level, *mid) +
                                       tree_projections(tree_level, *left_it)) / 2.0;
        }

        grow_subtree(begin, mid, tree_level + 1, idx_left, n_tree, tree_projections);
        grow_subtree(mid, end, tree_level + 1, idx_right, n_tree, tree_projections);
    }

    /**
    * Find k nearest neighbors from data for the query point
    */
    void exact_knn(const Eigen::Map<const Eigen::VectorXf> &q, int k, const Eigen::VectorXi &indices,
                   int n_elected, int *out, float *out_distances = nullptr) const {

        // 如果elected值为0，则直接返回-1，表示查询结束
        if (!n_elected) {
            for (int i = 0; i < k; ++i)
                out[i] = -1;

            if (out_distances) {
                for (int i = 0; i < k; ++i)
                    out_distances[i] = -1;
            }

            return;
        }

        Eigen::VectorXf distances(n_elected);

#pragma omp parallel for
        for (int i = 0; i < n_elected; ++i)    // 这里是选中的，在桶里的n个
            distances(i) = (X.col(indices(i)) - q).squaredNorm();    // squaredNorm是自身的内积（a*a + b*b）

        if (k == 1) {
            // 如果只找一个节点，直接查找，然后返回
            Eigen::MatrixXf::Index index;
            distances.minCoeff(&index);
            out[0] = n_elected ? indices(index) : -1;

            if (out_distances)
                // 上面计算出来内积之后，这里又给sqrt回去了
                out_distances[0] = n_elected ? std::sqrt(distances(index)) : -1;

            return;
        }

        int n_to_sort = n_elected > k ? k : n_elected;
        Eigen::VectorXi idx(n_elected);
        std::iota(idx.data(), idx.data() + n_elected, 0);

        // 找出前n_to_sort个内容
        std::partial_sort(idx.data(), idx.data() + n_to_sort, idx.data() + n_elected,
                          [&distances](int i1, int i2) { return distances(i1) < distances(i2); });

        for (int i = 0; i < k; ++i)
            out[i] = i < n_elected ? indices(idx(i)) : -1;

        if (out_distances) {
            for (int i = 0; i < k; ++i)
                out_distances[i] = i < n_elected ? std::sqrt(distances(idx(i))) : -1;
        }
    }

    void prune(double target_recall) {
        if (target_recall < 0.0 - epsilon || target_recall > 1.0 + epsilon) {
            throw std::out_of_range("Target recall must be on the interval [0,1].");
        }

        par = parameters(target_recall);
        if (!par.n_trees) {
            return;
        }

        int depth_max = depth;

        n_trees = par.n_trees;
        depth = par.depth;
        votes = par.votes;
        n_pool = depth * n_trees;
        n_array = 1 << (depth + 1);

        tree_leaves.resize(n_trees);
        tree_leaves.shrink_to_fit();
        split_points.conservativeResize(n_array, n_trees);
        leaf_first_indices = leaf_first_indices_all[depth];

        if (density < 1) {
            Eigen::SparseMatrix<float, Eigen::RowMajor> srm_new(n_pool, dim);
            for (int n_tree = 0; n_tree < n_trees; ++n_tree)
                srm_new.middleRows(n_tree * depth, depth) = sparse_random_matrix.middleRows(n_tree * depth_max, depth);
            sparse_random_matrix = srm_new;
        } else {
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> drm_new(n_pool, dim);
            for (int n_tree = 0; n_tree < n_trees; ++n_tree)
                drm_new.middleRows(n_tree * depth, depth) = dense_random_matrix.middleRows(n_tree * depth_max, depth);
            dense_random_matrix = drm_new;
        }

        index_type = autotuned;
    }

    void count_elected(const Eigen::VectorXf &q, const Eigen::Map<Eigen::VectorXi> &exact, int votes_max,
                       std::vector<Eigen::MatrixXd> &recalls, std::vector<Eigen::MatrixXd> &cs_sizes) const {
        Eigen::VectorXf projected_query(n_pool);
        if (density < 1)
            projected_query.noalias() = sparse_random_matrix * q;
        else
            projected_query.noalias() = dense_random_matrix * q;

        int depth_min = depth - recalls.size() + 1;
        std::vector<std::vector<int>> start_indices(n_trees);

#pragma omp parallel for
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            start_indices[n_tree] = std::vector<int>(depth - depth_min + 1);
            int idx_tree = 0;
            for (int d = 0; d < depth; ++d) {
                const int j = n_tree * depth + d;
                const int idx_left = 2 * idx_tree + 1;
                const int idx_right = idx_left + 1;
                const float split_point = split_points(idx_tree, n_tree);
                if (projected_query(j) <= split_point) {
                    idx_tree = idx_left;
                } else {
                    idx_tree = idx_right;
                }
                if (d >= depth_min - 1)
                    start_indices[n_tree][d - depth_min + 1] = idx_tree - (1 << (d + 1)) + 1;
            }
        }

        const int *exact_begin = exact.data();
        const int *exact_end = exact.data() + exact.size();

        for (int depth_crnt = depth_min; depth_crnt <= depth; ++depth_crnt) {
            Eigen::VectorXi votes = Eigen::VectorXi::Zero(n_samples);
            const std::vector<int> &leaf_first_indices = leaf_first_indices_all[depth_crnt];

            Eigen::MatrixXd recall(votes_max, n_trees);
            Eigen::MatrixXd candidate_set_size(votes_max, n_trees);
            recall.col(0) = Eigen::VectorXd::Zero(votes_max);
            candidate_set_size.col(0) = Eigen::VectorXd::Zero(votes_max);

            // count votes
            for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
                std::vector<int> &found_leaves = start_indices[n_tree];

                if (n_tree) {
                    recall.col(n_tree) = recall.col(n_tree - 1);
                    candidate_set_size.col(n_tree) = candidate_set_size.col(n_tree - 1);
                }

                int leaf_begin = leaf_first_indices[found_leaves[depth_crnt - depth_min]];
                int leaf_end = leaf_first_indices[found_leaves[depth_crnt - depth_min] + 1];

                const std::vector<int> &indices = tree_leaves[n_tree];
                for (int i = leaf_begin; i < leaf_end; ++i) {
                    int idx = indices[i];
                    int v = ++votes(idx);
                    if (v <= votes_max) {
                        candidate_set_size(v - 1, n_tree)++;
                        if (std::find(exact_begin, exact_end, idx) != exact_end)
                            recall(v - 1, n_tree)++;
                    }
                }
            }

            recalls[depth_crnt - depth_min] = recall;
            cs_sizes[depth_crnt - depth_min] = candidate_set_size;
        }
    }

    /**
    * Builds a random sparse matrix for use in random projection. The components of
    * the matrix are drawn from the distribution
    *
    *       0 w.p. 1 - a
    * N(0, 1) w.p. a
    *
    * where a = density.
    */
    static void build_sparse_random_matrix(Eigen::SparseMatrix<float, Eigen::RowMajor> &sparse_random_matrix,
                                           int n_row, int n_col, float density, int seed = 0) {
        sparse_random_matrix = Eigen::SparseMatrix<float, Eigen::RowMajor>(n_row, n_col);

        std::random_device rd;
        int s = seed ? seed : rd();
        std::mt19937 gen(s);
        std::uniform_real_distribution<float> uni_dist(0, 1);
        std::normal_distribution<float> norm_dist(0, 1);

        std::vector<Eigen::Triplet<float>> triplets;
        for (int j = 0; j < n_row; ++j) {
            for (int i = 0; i < n_col; ++i) {
                if (uni_dist(gen) > density) continue;
                triplets.push_back(Eigen::Triplet<float>(j, i, norm_dist(gen)));
            }
        }

        sparse_random_matrix.setFromTriplets(triplets.begin(), triplets.end());
        sparse_random_matrix.makeCompressed();
    }

    /*
    * Builds a random dense matrix for use in random projection. The components of
    * the matrix are drawn from the standard normal distribution.
    */
    static void build_dense_random_matrix(
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> &dense_random_matrix,
            int n_row, int n_col, int seed = 0) {
        dense_random_matrix = Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>(n_row, n_col);

        std::random_device rd;
        int s = seed ? seed : rd();
        std::mt19937 gen(s);
        std::normal_distribution<float> normal_dist(0, 1);

        std::generate(dense_random_matrix.data(), dense_random_matrix.data() + n_row * n_col,
                      [&normal_dist, &gen] { return normal_dist(gen); });
    }

    void compute_exact(const Eigen::Map<const Eigen::MatrixXf> &Q, Eigen::MatrixXi &out_exact,
                       const std::vector<int> &indices_test = {}) const {
        int n_test = Q.cols();

        Eigen::VectorXi idx(n_samples);
        std::iota(idx.data(), idx.data() + n_samples, 0);

        for (int i = 0; i < n_test; ++i) {
            if (!indices_test.empty()) {
                std::remove(idx.data(), idx.data() + n_samples, indices_test[i]);
            }
            exact_knn(Eigen::Map<const Eigen::VectorXf>(Q.data() + i * dim, dim), k, idx,
                      (indices_test.empty() ? n_samples : n_samples - 1), out_exact.data() + i * k);
            std::sort(out_exact.data() + i * k, out_exact.data() + i * k + k);
            if (!indices_test.empty()) {
                idx[n_samples - 1] = indices_test[i];
            }
        }
    }

    static bool is_faster(const Mrpt_Parameters &par1, const Mrpt_Parameters &par2) {
        return par1.estimated_qtime < par2.estimated_qtime;
    }

    void vote(const Eigen::VectorXf &projected_query, int vote_threshold, Eigen::VectorXi &elected,
              int &n_elected, int n_trees, int depth_crnt) {
        std::vector<int> found_leaves(n_trees);
        const std::vector<int> &leaf_first_indices = leaf_first_indices_all[depth_crnt];

#pragma omp parallel for
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            int idx_tree = 0;
            for (int d = 0; d < depth_crnt; ++d) {
                const int j = n_tree * depth + d;
                const int idx_left = 2 * idx_tree + 1;
                const int idx_right = idx_left + 1;
                const float split_point = split_points(idx_tree, n_tree);
                if (projected_query(j) <= split_point) {
                    idx_tree = idx_left;
                } else {
                    idx_tree = idx_right;
                }
            }
            found_leaves[n_tree] = idx_tree - (1 << depth_crnt) + 1;
        }

        int max_leaf_size = n_samples / (1 << depth_crnt) + 1;
        elected = Eigen::VectorXi(n_trees * max_leaf_size);
        Eigen::VectorXi votes = Eigen::VectorXi::Zero(n_samples);

        // count votes
        for (int n_tree = 0; n_tree < n_trees; ++n_tree) {
            int leaf_begin = leaf_first_indices[found_leaves[n_tree]];
            int leaf_end = leaf_first_indices[found_leaves[n_tree] + 1];
            const std::vector<int> &indices = tree_leaves[n_tree];
            for (int i = leaf_begin; i < leaf_end; ++i) {
                int idx = indices[i];
                if (++votes(idx) == vote_threshold)
                    elected(n_elected++) = idx;
            }
        }
    }

    std::pair<double, double> fit_projection_times(const Eigen::Map<const Eigen::MatrixXf> &Q,
                                                   std::vector<int> &exact_x) {
        std::vector<double> projection_times, projection_x;
        long double idx_sum = 0;

        std::vector<int> tested_trees{1, 2, 3, 4, 5, 7, 10, 15, 20, 25, 30, 40, 50};
        generate_x(tested_trees, n_trees, 10, n_trees);

        for (int d = depth_min; d <= depth; ++d) {
            for (int i = 0; i < (int) tested_trees.size(); ++i) {
                int t = tested_trees[i];
                int n_random_vectors = t * d;
                projection_x.push_back(n_random_vectors);
                Eigen::SparseMatrix<float, Eigen::RowMajor> sparse_mat;
                Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> dense_mat;

                if (density < 1) {
                    build_sparse_random_matrix(sparse_mat, n_random_vectors, dim, density);
                } else {
                    build_dense_random_matrix(dense_mat, n_random_vectors, dim);
                }

                double start_proj = omp_get_wtime();
                Eigen::VectorXf projected_query(n_random_vectors);

                if (density < 1) {
                    projected_query.noalias() = sparse_mat * Q.col(0);
                } else {
                    projected_query.noalias() = dense_mat * Q.col(0);
                }

                double end_proj = omp_get_wtime();
                projection_times.push_back(end_proj - start_proj);
                idx_sum += projected_query.norm();

                int votes_index = votes_max < t ? votes_max : t;
                for (int v = 1; v <= votes_index; ++v) {
                    int cs_size = get_candidate_set_size(t, d, v);
                    if (cs_size > 0) exact_x.push_back(cs_size);
                }
            }
        }

        // use results to ensure that the compiler does not optimize away the timed code.
        projection_x[0] += idx_sum > 1.0 ? 0.0000 : 0.0001;
        return fit_theil_sen(projection_x, projection_times);
    }

    std::vector<std::map<int, std::pair<double, double>>> fit_voting_times(const Eigen::Map<const Eigen::MatrixXf> &Q) {
        int n_test = Q.cols();

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, n_test - 1);

        std::vector<int> tested_trees{1, 2, 3, 4, 5, 7, 10, 15, 20, 25, 30, 40, 50};
        generate_x(tested_trees, n_trees, 10, n_trees);
        std::vector<int> vote_thresholds_x{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        generate_x(vote_thresholds_x, votes_max, 10, votes_max);

        beta_voting = std::vector<std::map<int, std::pair<double, double>>>();

        for (int d = depth_min; d <= depth; ++d) {
            std::map<int, std::pair<double, double>> beta;
            for (const auto &v : vote_thresholds_x) {
                long double idx_sum = 0;
                std::vector<double> voting_times, voting_x;

                for (int i = 0; i < (int) tested_trees.size(); ++i) {
                    int t = tested_trees[i];
                    int n_el = 0;
                    Eigen::VectorXi elected;
                    auto ri = uni(rng);

                    Eigen::VectorXf projected_query(n_trees *depth);
                    if (density < 1) {
                        projected_query.noalias() = sparse_random_matrix * Q.col(ri);
                    } else {
                        projected_query.noalias() = dense_random_matrix * Q.col(ri);
                    }

                    double start_voting = omp_get_wtime();
                    vote(projected_query, v, elected, n_el, t, d);
                    double end_voting = omp_get_wtime();

                    voting_times.push_back(end_voting - start_voting);
                    voting_x.push_back(t);
                    for (int i = 0; i < n_el; ++i)
                        idx_sum += elected(i);
                }
                voting_x[0] += idx_sum > 1.0 ? 0.0 : 0.00001;
                beta[v] = fit_theil_sen(voting_x, voting_times);
            }
            beta_voting.push_back(beta);
        }

        return beta_voting;
    }

    static void generate_x(std::vector<int> &x, int max_generated, int n_tested, int max_val) {
        n_tested = max_generated > n_tested ? n_tested : max_val;
        int increment = max_generated / n_tested;
        for (int i = 1; i <= n_tested; ++i) {
            if (std::find(x.begin(), x.end(), i * increment) == x.end() && i * increment <= max_generated) {
                x.push_back(i * increment);
            }
        }

        auto end = std::remove_if(x.begin(), x.end(), [max_val](int t) { return t > max_val; });
        x.erase(end, x.end());
    }

    std::pair<double, double> fit_exact_times(const Eigen::Map<const Eigen::MatrixXf> &Q) {
        std::vector<int> s_tested{1, 2, 5, 10, 20, 35, 50, 75, 100, 150, 200, 300, 400, 500};
        generate_x(s_tested, n_samples / 20, 20, n_samples);

        int n_test = Q.cols();
        std::vector<double> exact_times;
        long double idx_sum = 0;

        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, n_test - 1);
        std::uniform_int_distribution<int> uni2(0, n_samples - 1);

        std::vector<double> ex;
        int n_sim = 20;
        for (int i = 0; i < (int) s_tested.size(); ++i) {
            double mean_exact_time = 0;
            int s_size = s_tested[i];
            ex.push_back(s_size);

            for (int m = 0; m < n_sim; ++m) {
                auto ri = uni(rng);
                Eigen::VectorXi elected(s_size);
                for (int j = 0; j < elected.size(); ++j)
                    elected(j) = uni2(rng);

                double start_exact = omp_get_wtime();
                std::vector<int> res(k);
                exact_knn(Eigen::Map<const Eigen::VectorXf>(Q.data() + ri * dim, dim), k, elected, s_size, &res[0]);
                double end_exact = omp_get_wtime();
                mean_exact_time += (end_exact - start_exact);

                for (int l = 0; l < k; ++l)
                    idx_sum += res[l];
            }
            mean_exact_time /= n_sim;
            exact_times.push_back(mean_exact_time);
        }

        ex[0] += idx_sum > 1.0 ? 0.0 : 0.00001;
        return fit_theil_sen(ex, exact_times);
    }

    std::set<Mrpt_Parameters, decltype(is_faster) *> list_parameters(const std::vector<Eigen::MatrixXd> &recalls) {
        std::set<Mrpt_Parameters, decltype(is_faster) *> pars(is_faster);
        std::vector<Eigen::MatrixXd> query_times(depth - depth_min + 1);
        for (int d = depth_min; d <= depth; ++d) {
            Eigen::MatrixXd query_time = Eigen::MatrixXd::Zero(votes_max, n_trees);

            for (int t = 1; t <= n_trees; ++t) {
                int votes_index = votes_max < t ? votes_max : t;
                for (int v = 1; v <= votes_index; ++v) {
                    double qt = get_query_time(t, d, v);
                    query_time(v - 1, t - 1) = qt;
                    Mrpt_Parameters p;
                    p.n_trees = t;
                    p.depth = d;
                    p.votes = v;
                    p.k = k;
                    p.estimated_qtime = qt;
                    p.estimated_recall = recalls[d - depth_min](v - 1, t - 1);
                    pars.insert(p);
                }
            }

            query_times[d - depth_min] = query_time;
        }

        return pars;
    }

    std::set<Mrpt_Parameters, decltype(is_faster) *>
    pareto_frontier(const std::set<Mrpt_Parameters, decltype(is_faster) *> &pars) {
        opt_pars = std::set<Mrpt_Parameters, decltype(is_faster) *>(is_faster);
        double best_recall = -1.0;
        for (const auto &p : pars) { // compute pareto frontier for query times and recalls
            if (p.estimated_recall > best_recall) {
                opt_pars.insert(p);
                best_recall = p.estimated_recall;
            }
        }

        return opt_pars;
    }

    void fit_times(const Eigen::Map<const Eigen::MatrixXf> &Q) {
        std::vector<int> exact_x;
        beta_projection = fit_projection_times(Q, exact_x);
        beta_voting = fit_voting_times(Q);
        beta_exact = fit_exact_times(Q);
    }

    static std::pair<double, double> fit_theil_sen(const std::vector<double> &x,
                                                   const std::vector<double> &y) {
        int n = x.size();
        std::vector<double> slopes;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i != j)
                    slopes.push_back((y[j] - y[i]) / (x[j] - x[i]));
            }
        }

        int n_slopes = slopes.size();
        std::nth_element(slopes.begin(), slopes.begin() + n_slopes / 2, slopes.end());
        double slope = *(slopes.begin() + n_slopes / 2);

        std::vector<double> residuals(n);
        for (int i = 0; i < n; ++i)
            residuals[i] = y[i] - slope * x[i];

        std::nth_element(residuals.begin(), residuals.begin() + n / 2, residuals.end());
        double intercept = *(residuals.begin() + n / 2);

        return std::make_pair(intercept, slope);
    }

    void write_parameters(const Mrpt_Parameters *p, FILE *fd) const {
        if (!fd) {
            return;
        }

        fwrite(&p->n_trees, sizeof(int), 1, fd);
        fwrite(&p->depth, sizeof(int), 1, fd);
        fwrite(&p->votes, sizeof(int), 1, fd);
        fwrite(&p->k, sizeof(int), 1, fd);
        fwrite(&p->estimated_qtime, sizeof(double), 1, fd);
        fwrite(&p->estimated_recall, sizeof(double), 1, fd);
    }

    void read_parameters(Mrpt_Parameters *p, FILE *fd) {
        fread(&p->n_trees, sizeof(int), 1, fd);
        fread(&p->depth, sizeof(int), 1, fd);
        fread(&p->votes, sizeof(int), 1, fd);
        fread(&p->k, sizeof(int), 1, fd);
        fread(&p->estimated_qtime, sizeof(double), 1, fd);
        fread(&p->estimated_recall, sizeof(double), 1, fd);
    }

    void write_parameter_list(const std::set<Mrpt_Parameters, decltype(is_faster) *> &pars, FILE *fd) const {
        if (!fd) {
            return;
        }

        int par_sz = pars.size();
        fwrite(&par_sz, sizeof(int), 1, fd);

        for (const auto p : pars)
            write_parameters(&p, fd);
    }

    void read_parameter_list(FILE *fd) {
        if (!fd) {
            return;
        }

        opt_pars = std::set<Mrpt_Parameters, decltype(is_faster) *>(is_faster);
        int par_sz = 0;
        fread(&par_sz, sizeof(int), 1, fd);

        for (int i = 0; i < par_sz; ++i) {
            Mrpt_Parameters p;
            read_parameters(&p, fd);
            opt_pars.insert(p);
        }
    }

    Mrpt_Parameters parameters(double target_recall) const {
        double tr = target_recall - epsilon;
        for (const auto &p : opt_pars) {
            if (p.estimated_recall > tr) {
                return p;
            }
        }

        if (!opt_pars.empty()) {
            return *(opt_pars.rbegin());
        }

        return Mrpt_Parameters();
    }

    /**
    * Computes the leaf sizes of a tree assuming a median split and that
    * when the number points is odd, the extra point is always assigned to
    * to the left branch.
    */
    static void count_leaf_sizes(int n, int level, int tree_depth, std::vector<int> &out_leaf_sizes) {
        if (level == tree_depth) {
            out_leaf_sizes.push_back(n);
            return;
        }

        count_leaf_sizes(n - n / 2, level + 1, tree_depth, out_leaf_sizes);
        count_leaf_sizes(n / 2, level + 1, tree_depth, out_leaf_sizes);
    }

    /**
    * Computes indices of the first elements of leaves in a vector containing
    * all the leaves of a tree concatenated. Assumes that median split is used
    * and when the number points is odd, the extra point is always assigned to
    * the left branch.
    */
    static void count_first_leaf_indices(std::vector<int> &indices, int n, int depth) {
        std::vector<int> leaf_sizes;
        count_leaf_sizes(n, 0, depth, leaf_sizes);

        indices = std::vector<int>(leaf_sizes.size() + 1);
        indices[0] = 0;
        for (int i = 0; i < (int) leaf_sizes.size(); ++i)
            indices[i + 1] = indices[i] + leaf_sizes[i];
    }

    static void count_first_leaf_indices_all(std::vector<std::vector<int>> &indices, int n, int depth_max) {
        for (int d = 0; d <= depth_max; ++d) {
            std::vector<int> idx;
            count_first_leaf_indices(idx, n, d);
            indices.push_back(idx);
        }
    }

    static double predict_theil_sen(double x, std::pair<double, double> beta) {
        return beta.first + beta.second * x;
    }

    double get_candidate_set_size(int tree, int depth, int v) const {
        return cs_sizes[depth - depth_min](v - 1, tree - 1);
    }

    double get_projection_time(int n_trees, int depth, int v) const {
        return predict_theil_sen(n_trees * depth, beta_projection);
    }

    double get_voting_time(int n_trees, int depth, int v) const {
        const std::map<int, std::pair<double, double>> &beta = beta_voting[depth - depth_min];

        if (v <= 0 || beta.empty()) {
            return 0.0;
        }

        for (const auto &b : beta) {
            if (v <= b.first) {
                return predict_theil_sen(n_trees, b.second);
            }
        }

        return predict_theil_sen(n_trees, beta.rbegin()->second);
    }

    double get_exact_time(int n_trees, int depth, int v) const {
        return predict_theil_sen(get_candidate_set_size(n_trees, depth, v), beta_exact);
    }

    double get_query_time(int tree, int depth, int v) const {
        return get_projection_time(tree, depth, v)
               + get_voting_time(tree, depth, v)
               + get_exact_time(tree, depth, v);
    }

    std::vector<int> sample_indices(int n_test, int seed = 0) const {
        std::random_device rd;
        int s = seed ? seed : rd();
        std::mt19937 gen(s);

        std::vector<int> indices_data(n_samples);
        std::iota(indices_data.begin(), indices_data.end(), 0);
        std::shuffle(indices_data.begin(), indices_data.end(), gen);
        return std::vector<int>(indices_data.begin(), indices_data.begin() + n_test);
    }

    Eigen::MatrixXf subset(const std::vector<int> &indices) const {
        int n_test = indices.size();
        Eigen::MatrixXf Q = Eigen::MatrixXf(dim, n_test);
        for (int i = 0; i < n_test; ++i)
            Q.col(i) = X.col(indices[i]);

        return Q;
    }


    Eigen::MatrixXf split_points; // all split points in all trees
    std::vector<std::vector<int>> tree_leaves; // contains all leaves of all trees
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> dense_random_matrix; // random vectors needed for all the RP-trees
    Eigen::SparseMatrix<float, Eigen::RowMajor> sparse_random_matrix; // random vectors needed for all the RP-trees
    std::vector<std::vector<int>> leaf_first_indices_all; // first indices for each level
    std::vector<int> leaf_first_indices; // first indices of each leaf of tree in tree_leaves

    const Eigen::Map<const Eigen::MatrixXf> X; // the data matrix
    const int n_samples; // sample size of data
    const int dim; // dimension of data

    Mrpt_Parameters par;
    int n_trees = 0; // number of RP-trees
    int depth = 0; // depth of an RP-tree with median split
    float density = -1.0; // expected ratio of non-zero components in a projection matrix
    int n_pool = 0; // amount of random vectors needed for all the RP-trees
    int n_array = 0; // length of the one RP-tree as array
    int votes = 0; // optimal number of votes to use
    int k = 0;
    enum itype {
        normal, autotuned, autotuned_unpruned
    };
    itype index_type = normal;

    // Member variables used in autotuning:
    int depth_min = 0;
    int votes_max = 0;
    const double epsilon = 0.0001; // error bound for comparisons of recall levels
    std::vector<Eigen::MatrixXd> cs_sizes;
    std::pair<double, double> beta_projection, beta_exact;
    std::vector<std::map<int, std::pair<double, double>>> beta_voting;
    std::set<Mrpt_Parameters, decltype(is_faster) *> opt_pars;
};



#endif //CAISS_MRPT_H
