/**
 * @author junfeng.fj
 * @Name JavaCaiss.java
 * @date 2020/9/15 1:23 上午
 * @Desc
 */

import com.sun.jna.Native;
import com.sun.jna.*;
import com.sun.jna.ptr.NativeLongByReference;
import com.sun.jna.ptr.PointerByReference;

public class JaCaiss {
    public static String CAISS_PATH = "caiss";    // 请设置正确caiss动态库的路径信息

    private interface JnaCaiss extends Library {
        JnaCaiss instance = (JnaCaiss) Native.loadLibrary(CAISS_PATH, JnaCaiss.class);

        int CAISS_Environment(NativeLong maxThreadSize, int algoType, int manageType);
        int CAISS_CreateHandle(PointerByReference pointer);
        int CAISS_Init(Pointer handle, int mode, int distanceType, NativeLong dim, String modelPath, Object distFunc);
        int CAISS_Train(Pointer handle, String dataPath, NativeLong maxDataSize, int normalize,
                        NativeLong maxIndexSize, float precision, NativeLong fastRank, NativeLong realRank,
                        NativeLong step, NativeLong maxEpoch, NativeLong showSpan);
        int CAISS_Search(Pointer handle, Pointer info, int searchType, NativeLong topK,
                         NativeLong filterEditDistance, Object searchCBFunc, Pointer cbParams);    // 暂不支持回调函数
        int CAISS_GetResultSize(Pointer handle, NativeLongByReference size);
        int CAISS_GetResult(Pointer handle, Pointer result, NativeLong size);
        int CAISS_Insert(Pointer handle, float[] node, String label, int insertType);
        int CAISS_Ignore(Pointer handle, String label, boolean isIgnore);
        int CAISS_Save(Pointer handle, String modelPath);
        int CAISS_ExecuteSQL(Pointer handle, String sql, Object sqlCBFunc, Pointer sqlParams);    // 暂不支持回调函数
        int CAISS_DestroyHandle(Pointer handle);
    }

    public int Environment(int maxThreadSize, int algoType, int manageType) {
        NativeLong maxThreadSize_ = new NativeLong(maxThreadSize);
        return JnaCaiss.instance.CAISS_Environment(maxThreadSize_, algoType, manageType);
    }

    public int CreateHandle(PointerByReference pointer) {
        return JnaCaiss.instance.CAISS_CreateHandle(pointer);
    }

    public int Init(Pointer handle, int mode, int distanceType, int dim, String modelPath) {
        NativeLong dim_ = new NativeLong(dim);
        return JnaCaiss.instance.CAISS_Init(handle, mode, distanceType, dim_, modelPath, null);
    }

    public int Train(Pointer handle, String dataPath, int maxDataSize, int normalize,
                     int maxIndexSize, float precision, int fastRank, int realRank,
                     int step, int maxEpoch, int showSpan) {
        NativeLong maxDataSize_ = new NativeLong(maxDataSize);
        NativeLong maxIndexSize_ = new NativeLong(maxIndexSize);
        NativeLong fastRank_ = new NativeLong(fastRank);
        NativeLong realRank_ = new NativeLong(realRank);
        NativeLong step_ = new NativeLong(step);
        NativeLong maxEpoch_ = new NativeLong(maxEpoch);
        NativeLong showSpan_ = new NativeLong(showSpan);
        return JnaCaiss.instance.CAISS_Train(handle, dataPath, maxDataSize_, normalize,
                maxIndexSize_, precision, fastRank_, realRank_, step_, maxEpoch_, showSpan_);
    }

    // 查询接口，适用于同步模式下的查询
    public String SyncSearch(Pointer handle, String info, int searchType, int topK,
                             int filterEditDistance , NativeLongByReference ref) {
        NativeLong filterEditDistance_ = new NativeLong(filterEditDistance);
        NativeLong topK_ = new NativeLong(topK);

        Pointer info_ = new Memory(info.length() + 1);    // todo 需要区分是向量查询，还是单词查询的模式
        info_.setString(0, info);

        int ret = JnaCaiss.instance.CAISS_Search(handle, info_, searchType, topK_,
                filterEditDistance_, null, null);

        Native.free(Pointer.nativeValue(info_));    // 释放申请的内存信息
        Pointer.nativeValue(info_, 0);

        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        return getResultString(handle, ref);
    }

    String SyncExecuteSQL(Pointer handle, String sql, NativeLongByReference ref) {
        int ret = JnaCaiss.instance.CAISS_ExecuteSQL(handle, sql, null, null);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        return getResultString(handle, ref);
    }

    public int Insert(Pointer handle, float[] node, String label, int insertType) {
        return JnaCaiss.instance.CAISS_Insert(handle, node, label, insertType);
    }

    public int Ignore(Pointer handle, String label, boolean isIgnore) {
        return JnaCaiss.instance.CAISS_Ignore(handle, label, isIgnore);
    }

    public int Save(Pointer handle, String modelPath) {
        return JnaCaiss.instance.CAISS_Save(handle, modelPath);
    }

    public int DestroyHandle(Pointer handle) {
        return JnaCaiss.instance.CAISS_DestroyHandle(handle);
    }

    /**
     * 获取最终的结果，并且返回对应的返回值
     * @param handle
     * @param ref
     * @return
     */
    private String getResultString(Pointer handle, NativeLongByReference ref) {
        NativeLongByReference sizeRef = new NativeLongByReference();
        int ret = JnaCaiss.instance.CAISS_GetResultSize(handle, sizeRef);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        long size = sizeRef.getValue().longValue();
        Pointer resultPtr = new Memory(size + 1);
        NativeLong size_ = new NativeLong(size);
        ret = JnaCaiss.instance.CAISS_GetResult(handle, resultPtr, size_);
        String result = (JaCaissDefine.CAISS_RET_OK == ret) ? resultPtr.getString(0) : "";

        ref.setValue(new NativeLong(ret));    // 设置返回值信息
        Native.free(Pointer.nativeValue(resultPtr));    // 释放申请的内存信息
        Pointer.nativeValue(resultPtr, 0);

        return result;    // 返回最终的查询结果信息
    }
}
