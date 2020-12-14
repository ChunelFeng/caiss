//
// Created by Chunel on 2020/5/24.
//


#include "SyncManageProc.h"

/**
 *
 * @param handle
 * @param size
 * @return
 */
CAISS_STATUS SyncManageProc::getResultSize(void *handle, unsigned int &size) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    ret = proc->getResultSize(size);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param result
 * @param size
 * @return
 */
CAISS_STATUS SyncManageProc::getResult(void *handle, char *result, const unsigned int size) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    ret = proc->getResult(result, size);
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param info
 * @param searchType
 * @param topK
 * @param filterEditDistance
 * @param searchCBFunc
 * @param cbParams
 * @return
 */
CAISS_STATUS SyncManageProc::search(void *handle, void *info, CAISS_SEARCH_TYPE searchType, unsigned int topK,
                                    const unsigned int filterEditDistance, const CAISS_SEARCH_CALLBACK searchCBFunc, const void *cbParams) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    // 查询的时候，使用读锁即可；插入的时候，需要使用写锁
    this->lock_.readLock();    // 在同步的时候，这个lock实际上就锁住proc这个信息了
    ret = proc->search(info, searchType, topK, filterEditDistance, searchCBFunc, cbParams);
    this->lock_.readUnlock();

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param dataPath
 * @param maxDataSize
 * @param normalize
 * @param maxIndexSize
 * @param precision
 * @param fastRank
 * @param realRank
 * @param step
 * @param maxEpoch
 * @param showSpan
 * @return
 */
CAISS_STATUS SyncManageProc::train(void *handle, const char *dataPath, const unsigned int maxDataSize,
                                   CAISS_BOOL normalize, const unsigned int maxIndexSize, const float precision,
                                   const unsigned int fastRank, const unsigned int realRank, const unsigned int step,
                                   const unsigned int maxEpoch, const unsigned int showSpan) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();    // 这里决定了，训练不支持多线程操作
    ret = proc->train(dataPath, maxDataSize, normalize, maxIndexSize, precision, fastRank, realRank, step, maxEpoch, showSpan);
    this->lock_.writeUnlock();

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param modelPath
 * @return
 */
CAISS_STATUS SyncManageProc::save(void *handle, const char *modelPath) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->save(modelPath);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param node
 * @param label
 * @param insertType
 * @return
 */
CAISS_STATUS SyncManageProc::insert(void *handle,
                                    CAISS_FLOAT *node,
                                    const char *label,
                                    CAISS_INSERT_TYPE insertType) {
    CAISS_FUNCTION_BEGIN

    /* 插入逻辑设计到写锁，还是使用同步的方式进行 */
    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();
    ret = proc->insert(node, label, insertType);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param label
 * @param isIgnore
 * @return
 */
CAISS_STATUS SyncManageProc::ignore(void *handle,
                                    const char *label,
                                    CAISS_BOOL isIgnore) {
    CAISS_FUNCTION_BEGIN

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    this->lock_.writeLock();    // 忽略label的功能，需要加写锁实现
    ret = proc->ignore(label, isIgnore);
    this->lock_.writeUnlock();
    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


/**
 *
 * @param handle
 * @param sql
 * @param sqlCBFunc
 * @param sqlParams
 * @return
 */
CAISS_STATUS SyncManageProc::executeSQL(void *handle,
                                        const char *sql,
                                        CAISS_SEARCH_CALLBACK sqlCBFunc,
                                        const void *sqlParams) {
    CAISS_FUNCTION_BEGIN

    CAISS_ASSERT_NOT_NULL(sql)

    AlgorithmProc *proc = this->getInstance(handle);
    CAISS_ASSERT_NOT_NULL(proc)

    SqlProc sqlProc;
    ret = sqlProc.parseSql(sql);
    CAISS_FUNCTION_CHECK_STATUS

    if (model_path_.find(sqlProc.getTableName()) == std::string::npos
        || sqlProc.getTableName().empty()) {
        return CAISS_RET_PATH;    // 当传入的模型名称，跟模型真实的名称不同的时候，
    }

    switch (sqlProc.getType()) {
        case hsql::kStmtSelect: {
            ret = this->search(handle, (void *)sqlProc.getQueryWord(),
                               CAISS_SEARCH_WORD, sqlProc.getLimitNum(), 0,
                               sqlCBFunc, sqlParams);
        }
            break;
        case hsql::kStmtInsert: {
            std::vector<std::string> keys = sqlProc.getInsertKeys();
            std::vector<void *> values = sqlProc.getInsertValues();
            int size = (int)keys.size();
            for (int i = 0; i < size; i++) {
                // 如果重复插入的话，则丢弃当前信息。可以批量插入信息
                ret = this->insert(handle, (CAISS_FLOAT *)values[i], keys[i].c_str(), CAISS_INSERT_DISCARD);
                CAISS_FUNCTION_CHECK_STATUS
            }
        }
            break;
        case hsql::kStmtUpdate: {
            // 更新信息。如果之前模型中没有，则直接插入
            ret = this->insert(handle, (CAISS_FLOAT *)sqlProc.getUpdateValue(), sqlProc.getUpdateInfo(),
                               CAISS_INSERT_OVERWRITE);
        }
            break;
        case hsql::kStmtDelete: {
            ret = this->ignore(handle, sqlProc.getDeleteQueryWord(), true);
        }
            break;
        default:
            ret = CAISS_RET_SQL_PARSE;    // 如果不是以上语句，则不支持
            break;
    }

    CAISS_FUNCTION_CHECK_STATUS

    CAISS_FUNCTION_END
}


