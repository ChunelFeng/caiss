/**
 * @author junfeng.fj
 * @Name CsCaissDefine.cs
 * @date 2020/9/20 1:23 上午
 * @Desc 具体函数功能，请参考说明文档
 */

using System;
using System.Runtime.InteropServices;

namespace CsCaissSpace
{
    public class CsCaissDefine {
        /* 函数返回值定义 */
        public const int CAISS_RET_NO_WORD      = 2;     // 模型词库中无对应词语问题
        public const int CAISS_RET_WARNING      = 1;     // 流程告警
        public const int CAISS_RET_OK           = 0;     // 流程正常
        public const int CAISS_RET_ERR          = -1;    // 流程异常
        public const int CAISS_RET_RES          = -2;    // 资源问题
        public const int CAISS_RET_MODE         = -3;    // 模式选择问题
        public const int CAISS_RET_PATH         = -4;    // 路径问题
        public const int CAISS_RET_JSON         = -5;    // json解析问题
        public const int CAISS_RET_PARAM        = -6;    // 参数问题
        public const int CAISS_RET_HANDLE       = -7;    // 句柄申请问题
        public const int CAISS_RET_DIM          = -8;    // 维度问题
        public const int CAISS_RET_MODEL_SIZE   = -9;    // 模型尺寸限制问题
        public const int CAISS_RET_WORD_SIZE    = -10;   // 词语长度限制问题
        public const int CAISS_RET_SQL_PARSE    = -11;   // 传入的sql无法解析
        public const int CAISS_RET_NO_SUPPORT   = -99;   // 暂不支持该功能

        public enum CAISS_MODE {
            CAISS_MODE_DEFAULT = 0,    // 无效模式
            CAISS_MODE_TRAIN = 1,      // 训练模式
            CAISS_MODE_PROCESS = 2,    // 处理模式
        };

        public enum CAISS_SEARCH_TYPE {
            CAISS_SEARCH_DEFAULT = 0,
            CAISS_SEARCH_QUERY = 1,    // 通过快速检索的方式，查询query信息
            CAISS_SEARCH_WORD = 2,     // 通过快速检索的方式，查询word信息
            CAISS_LOOP_QUERY = 3,      // 通过暴力循环的方式，查询query信息
            CAISS_LOOP_WORD = 4        // 通过暴力循环的方式，查询word信息
        };

        public enum CAISS_INSERT_TYPE {
            // 如果插入相同的数据
            CAISS_INSERT_DEFAULT = 1,
            CAISS_INSERT_OVERWRITE = 1,   // 覆盖模型中原有的数据
            CAISS_INSERT_DISCARD = 2,     // 丢弃当前插入的数据
        };

        public enum CAISS_MANAGE_TYPE {
            CAISS_MANAGE_DEFAULT = 1,
            CAISS_MANAGE_SYNC = 1,     // 同步控制
            CAISS_MANAGE_ASYNC = 2,    // 异步控制
        };

        public enum CAISS_DISTANCE_TYPE {
            CAISS_DISTANCE_DEFAULT = 1,
            CAISS_DISTANCE_EUC = 1,         // 欧氏距离
            CAISS_DISTANCE_INNER = 2,       // 内积距离
            CAISS_DISTANCE_JACCARD = 3,     // 杰卡德距离
            CAISS_DISTANCE_EDITION = 99,    // 自定义距离（注：设定自定义距离时，必须是较小的值，表示较为接近）
        };

        public enum CAISS_ALGO_TYPE {
            CAISS_ALGO_DEFAULT = 1,
            CAISS_ALGO_HNSW = 1,            // hnsw算法（准确度高，空间复杂度较大）
            CAISS_ALGO_NSG = 2              // nsg算法（准确度较高，空间复杂度小）
        };
        
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate float CAISS_DIST_FUNC(Object vec1, Object vec2, Object params_);
        
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate float CAISS_SEARCH_CALLBACK(Object words, Object distances, Object params_);
        
        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public delegate float CAISS_SQL_CALLBACK(Object words, Object distances, Object params_);
    }
}