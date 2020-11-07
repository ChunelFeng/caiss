/**
 * @author Chunel
 * @Name JavaCaissDemo.java
 * @date 2020/9/15 9:13 下午
 * @Desc
 */

import com.sun.jna.Pointer;
import com.sun.jna.ptr.NativeLongByReference;
import com.sun.jna.ptr.PointerByReference;

public class JaCaissDemo {
    public static String DATA_PATH = "bert_71290words_768dim.txt";
    public static int MAX_DATA_SIZE = 100000;
    public static int NORMALIZE = 1;    // 是否将向量信息标准化
    public static int MAX_INDEX_SIZE = 64;
    public static float PRECISION = 0.95f;
    public static int FAST_RANK = 5;
    public static int REAL_RANK = 5;
    public static int STEP = 1;
    public static int MAX_EPOCH = 5;
    public static int SHOW_SPAN = 1000;

    public static String MODEL_PATH = "bert_71290words_768dim.caiss";
    public static int MAX_THREAD_SIZE = 1;
    public static int TOP_K = 5;
    public static int FILTER_EDIT_DISTANCE = 0;
    public static int DIM = 768;
    public static String QUERY_WORD = "water";
    public static String QUERY_SQL = "SELECT * FROM bert_71290words_768dim WHERE word LIKE 'water' limit 5";

    public static int TrainDemo() {
        // 训练demo
        JaCaiss caiss = new JaCaiss();
        PointerByReference ptr = new PointerByReference(Pointer.NULL);
        int ret = caiss.CreateHandle(ptr);    // 申请句柄信息
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            return ret;
        }

        ret = caiss.Init(ptr.getValue(),    // 初始化查询状态
                JaCaissDefine.CAISS_MODE.CAISS_MODE_TRAIN,
                JaCaissDefine.CAISS_DISTANCE_TYPE.CAISS_DISTANCE_INNER,
                DIM, MODEL_PATH);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            return ret;
        }

        ret = caiss.Train(ptr.getValue(), DATA_PATH, MAX_DATA_SIZE, NORMALIZE, MAX_INDEX_SIZE,
                PRECISION, FAST_RANK, REAL_RANK, STEP, MAX_EPOCH, SHOW_SPAN);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            return ret;
        }

        ret = caiss.DestroyHandle(ptr.getValue());
        return ret;
    }

    public static int SearchDemo() {
        // 查询demo
        JaCaiss caiss = new JaCaiss();
        PointerByReference ptr = new PointerByReference(Pointer.NULL);
        int ret = caiss.CreateHandle(ptr);    // 申请句柄信息
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            return ret;
        }

        ret = caiss.Init(ptr.getValue(),    // 初始化查询状态
                JaCaissDefine.CAISS_MODE.CAISS_MODE_PROCESS,
                JaCaissDefine.CAISS_DISTANCE_TYPE.CAISS_DISTANCE_INNER,
                DIM, MODEL_PATH);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            System.out.println(ret);
            return ret;
        }

        NativeLongByReference ref = new NativeLongByReference();    // 根据ref返回的值，来判断SyncSearch函数是否执行成功
        // 使用常规查询方式
        String result = caiss.SyncSearch(ptr.getValue(), QUERY_WORD,
                JaCaissDefine.CAISS_SEARCH_TYPE.CAISS_SEARCH_WORD, TOP_K, FILTER_EDIT_DISTANCE, ref);
        if (JaCaissDefine.CAISS_RET_OK != ref.getPointer().getInt(0)) {
            return ref.getPointer().getInt(0);    // 返回异常值信息
        }
        System.out.println(result);

        // 使用sql查询方式
        result = caiss.SyncExecuteSQL(ptr.getValue(), QUERY_SQL, ref);
        if (JaCaissDefine.CAISS_RET_OK != ref.getPointer().getInt(0)) {
            return ref.getPointer().getInt(0);
        }
        System.out.println(result);

        ret = caiss.DestroyHandle(ptr.getValue());    // 析构句柄信息
        return ret;
    }

    public static void main(String[] args) {
        int ret = JaCaiss.Environment(MAX_THREAD_SIZE,
                JaCaissDefine.CAISS_ALGO_TYPE.CAISS_ALGO_HNSW,
                JaCaissDefine.CAISS_MANAGE_TYPE.CAISS_MANAGE_SYNC);
        if (JaCaissDefine.CAISS_RET_OK != ret) {
            System.out.println(ret);    // 如果处理异常，则打印错误码，并返回
            return;
        }

        // ret = TrainDemo();
        // System.out.println("Train Demo return is : " + ret);

        ret = SearchDemo();
        System.out.println("Search Demo return is : " + ret);
    }
}
