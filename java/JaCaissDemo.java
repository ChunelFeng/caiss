/**
 * @author junfeng.fj
 * @Name JavaCaissDemo.java
 * @date 2020/9/15 7:13 下午
 * @Desc
 */

import com.sun.jna.Memory;
import com.sun.jna.Native;
import com.sun.jna.NativeLong;
import com.sun.jna.Pointer;
import com.sun.jna.ptr.NativeLongByReference;
import com.sun.jna.ptr.PointerByReference;

public class JaCaissDemo {
    public static String MODEL_PATH = "bert_71290words_768dim.caiss";
    public static int MAX_THREAD_SIZE = 1;
    public static int TOP_K = 5;
    public static int FILTER_EDIT_DISTANCE = 0;
    public static int DIM = 768;
    public static String QUERY_WORD = "water";

    public static void main(String[] args) {
        int ret = JaCaissDefine.CAISS_RET_OK;
        JaCaiss caiss = new JaCaiss();
        ret = caiss.Environment(MAX_THREAD_SIZE,
                JaCaissDefine.CAISS_ALGO_TYPE.CAISS_ALGO_HNSW,
                JaCaissDefine.CAISS_MANAGE_TYPE.CAISS_MANAGE_SYNC);

        PointerByReference ptr = new PointerByReference(Pointer.NULL);
        ret = caiss.CreateHandle(ptr);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            System.out.println(ret);
            return;
        }

        ret = caiss.Init(ptr.getValue(),
                JaCaissDefine.CAISS_MODE.CAISS_MODE_PROCESS,
                JaCaissDefine.CAISS_DISTANCE_TYPE.CAISS_DISTANCE_INNER,
                DIM, MODEL_PATH);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            System.out.println(ret);
            return;
        }

        NativeLongByReference ref = new NativeLongByReference();    // 根据ref返回的值，来判断SyncSearch函数是否执行成功
        String result = caiss.SyncSearch(ptr.getValue(), QUERY_WORD,
                JaCaissDefine.CAISS_SEARCH_TYPE.CAISS_SEARCH_WORD, TOP_K, FILTER_EDIT_DISTANCE, ref);
        if (JaCaissDefine.CAISS_RET_OK != ref.getPointer().getInt(0)) {
            System.out.println(ref.getPointer().getInt(0));
            return;
        }
        System.out.println(result);

        ret = caiss.DestroyHandle(ptr.getValue());
    }
}
