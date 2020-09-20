/**
 * @author junfeng.fj
 * @Name CsCaissDemo.cs
 * @date 2020/9/20 1:23 上午
 * @Desc 具体函数功能，请参考说明文档
 */

using System;

namespace CsCaissSpace
{
    internal class CsCaissDemo
    {
        private static string DATA_PATH = "bert_71290words_768dim.txt";
        private static uint MAX_DATA_SIZE = 100000;
        private static int NORMALIZE = 1;    // 是否将向量信息标准化
        private static uint MAX_INDEX_SIZE = 64;
        private static float PRECISION = 0.95f;
        private static uint FAST_RANK = 5;
        private static uint REAL_RANK = 5;
        private static uint STEP = 1;
        private static uint MAX_EPOCH = 5;
        private static uint SHOW_SPAN = 1000;

        private static string MODEL_PATH = "bert_71290words_768dim.caiss";
        private static uint MAX_THREAD_SIZE = 1;
        private static uint TOP_K = 5;
        private static int FILTER_EDIT_DISTANCE = 0;
        private static uint DIM = 768;
        private static string QUERY_WORD = "water";
        private static string QUERY_SQL = "SELECT * FROM bert_71290words_768dim WHERE word LIKE 'water' limit 5";

        
        private static int TrainDemo()
        {
            // 训练demo
            var handle = IntPtr.Zero;
            var ret = CsCaiss.CreateHandle(ref handle);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                return ret;
            }

            ret = CsCaiss.Init(handle,
                CsCaissDefine.CAISS_MODE.CAISS_MODE_TRAIN,
                CsCaissDefine.CAISS_DISTANCE_TYPE.CAISS_DISTANCE_INNER,
                DIM, MODEL_PATH);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                return ret;
            }

            ret = CsCaiss.Train(handle, DATA_PATH, MAX_DATA_SIZE, NORMALIZE,
                MAX_INDEX_SIZE, PRECISION, FAST_RANK, REAL_RANK, STEP, MAX_EPOCH, SHOW_SPAN);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                return ret;
            }
            
            return CsCaiss.DestroyHandle(handle);
        }

        private static int SearchDemo()
        {
            // 查询demo
            var handle = IntPtr.Zero;
            var ret = CsCaiss.CreateHandle(ref handle);

            ret = CsCaiss.Init(handle,
                CsCaissDefine.CAISS_MODE.CAISS_MODE_PROCESS,
                CsCaissDefine.CAISS_DISTANCE_TYPE.CAISS_DISTANCE_INNER,
                DIM, MODEL_PATH);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                return ret;
            }

            int status = 0;
            string result = CsCaiss.SyncSearch(handle, QUERY_WORD, CsCaissDefine.CAISS_SEARCH_TYPE.CAISS_SEARCH_WORD, TOP_K,
                FILTER_EDIT_DISTANCE, ref status);
            //var result = CsCaiss.SyncExecuteSQL(handle, QUERY_SQL, ref status);    // sql方式查询
            if (CsCaissDefine.CAISS_RET_OK != status)
            {
                ret = status;
                return ret;
            }
            
            Console.WriteLine(result);    // 打印输出的结果信息
            
            return CsCaiss.DestroyHandle(handle);
        }
        
        public static void Main(string[] args)
        {
            // 初始化caiss环境
            var ret = CsCaiss.Environment(MAX_THREAD_SIZE,
                CsCaissDefine.CAISS_ALGO_TYPE.CAISS_ALGO_HNSW,
                CsCaissDefine.CAISS_MANAGE_TYPE.CAISS_MANAGE_SYNC);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                // caiss环境初始化异常
                return;
            }

            // ret = TrainDemo();
            // Console.WriteLine("train demo ret is : {0}", ret);    // 查看查询demo的返回值信息    // 查看训练demo的返回值信息

            ret = SearchDemo();
            Console.WriteLine("search demo ret is : {0}", ret);    // 查看查询demo的返回值信息
        }
    }
}