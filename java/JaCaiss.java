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
    public static String CAISS = "caiss";    // 设置正确的路径信息

    private interface JnaCaiss extends Library {
        JnaCaiss instance = (JnaCaiss) Native.loadLibrary(CAISS, JnaCaiss.class);

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

    public String SyncSearch(Pointer handle, String info, int searchType, int topK,
                             int filterEditDistance , NativeLongByReference ref) {
        NativeLong filterEditDistance_ = new NativeLong(filterEditDistance);
        NativeLong topK_ = new NativeLong(topK);

        Pointer info_ = new Memory(info.length() + 1);
        info_.setString(0, info);

        int ret = JnaCaiss.instance.CAISS_Search(handle, info_, searchType, topK_,
                filterEditDistance_, null, null);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        NativeLongByReference sizeRef_ = new NativeLongByReference();
        ret = JnaCaiss.instance.CAISS_GetResultSize(handle, sizeRef_);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        int size = (int)sizeRef_.getValue().longValue();
        Pointer result = new Memory(size + 1);
        NativeLong size_ = new NativeLong(size);
        ret = JnaCaiss.instance.CAISS_GetResult(handle, result, size_);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            ref.setValue(new NativeLong(ret));
            return "";
        }

        return result.getString(0);
    }

    public int DestroyHandle(Pointer handle) {
        return JnaCaiss.instance.CAISS_DestroyHandle(handle);
    }
}
