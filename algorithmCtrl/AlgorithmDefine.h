//
// Created by Chunel on 2020/11/28.
//

#ifndef CAISS_ALGORITHMDEFINE_H
#define CAISS_ALGORITHMDEFINE_H

#if _USE_EIGEN3_
    #include <Eigen/Core>
    typedef Eigen::Map<Eigen::Array<float, 1, Eigen::Dynamic>> DynamicArrayType;
#endif

#endif //CAISS_ALGORITHMDEFINE_H
