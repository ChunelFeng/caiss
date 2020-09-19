/**
 * @author junfeng.fj
 * @Name JavaCaissDefine.java
 * @date 2020/9/15 12:53 下午
 * @Desc
 */

public class JaCaissDefine {
    public static int CAISS_RET_NO_WORD     =  2;     // 模型词库中无对应词语问题
    public static int CAISS_RET_WARNING     =  1;     // 流程告警
    public static int CAISS_RET_OK          =  0;     // 流程正常
    public static int CAISS_RET_ERR         =  -1;    // 流程异常
    public static int CAISS_RET_RES         =  -2;    // 资源问题
    public static int CAISS_RET_MODE        =  -3;    // 模式选择问题
    public static int CAISS_RET_PATH        =  -4;    // 路径问题
    public static int CAISS_RET_JSON        =  -5;    // json解析问题
    public static int CAISS_RET_PARAM       =  -6;    // 参数问题
    public static int CAISS_RET_HANDLE      =  -7;    // 句柄申请问题
    public static int CAISS_RET_DIM         =  -8;    // 维度问题
    public static int CAISS_RET_MODEL_SIZE  =  -9;    // 模型尺寸限制问题
    public static int CAISS_RET_WORD_SIZE   =  -10;   // 词语长度限制问题
    public static int CAISS_RET_SQL_PARSE   =  -11;   // 传入的sql无法解析
    public static int CAISS_RET_NO_SUPPORT  =  -99;   // 暂不支持该功能

    public interface CAISS_MANAGE_TYPE {
        int CAISS_MANAGE_DEFAULT = 1;
        int CAISS_MANAGE_SYNC = 1;     // 同步控制
        int CAISS_MANAGE_ASYNC = 2;    // 异步控制
    }

    public interface CAISS_ALGO_TYPE {
        int CAISS_ALGO_DEFAULT = 1;
        int CAISS_ALGO_HNSW = 1;    // hnsw算法（准确度高，空间复杂度较大）
        int CAISS_ALGO_NSG = 2;     // nsg算法（准确度较高，空间复杂度小，暂不支持）
    }

    public interface CAISS_DISTANCE_TYPE {
        int CAISS_DISTANCE_DEFAULT = 1;
        int CAISS_DISTANCE_EUC = 1;         // 欧氏距离
        int CAISS_DISTANCE_INNER = 2;       // 内积距离
        int CAISS_DISTANCE_JACCARD = 3;     // 杰卡德距离
    }

    public interface CAISS_MODE {
        int CAISS_MODE_DEFAULT = 0;    // 无效模式
        int CAISS_MODE_TRAIN = 1;      // 训练模式
        int CAISS_MODE_PROCESS = 2;    // 处理模式
    }

    public interface CAISS_SEARCH_TYPE {
        int CAISS_SEARCH_DEFAULT = 0;
        int CAISS_SEARCH_QUERY = 1;     // 通过快速检索的方式，查询query信息
        int CAISS_SEARCH_WORD = 2;      // 通过快速检索的方式，查询word信息
        int CAISS_LOOP_QUERY = 3;       // 通过暴力循环的方式，查询query信息
        int CAISS_LOOP_WORD = 4;        // 通过暴力循环的方式，查询word信息
    }

    public interface  CAISS_INSERT_TYPE {
        // 如果插入相同的数据
        int CAISS_INSERT_DEFAULT = 1;
        int CAISS_INSERT_OVERWRITE = 1;   // 覆盖模型中原有的数据
        int CAISS_INSERT_DISCARD = 2;     // 丢弃当前插入的数据
    }
}
