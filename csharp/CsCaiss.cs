/**
 * @author junfeng.fj
 * @Name CsCaiss.cs
 * @date 2020/9/20 1:23 上午
 * @Desc 具体函数功能，请参考说明文档
 */

using System;
using System.Runtime.InteropServices;

namespace CsCaissSpace
{
    public class CsCaiss
    {
        private const string CAISS_LIB_PATH = "Caiss";    // 使用前，请输入正确的Caiss动态库路径，如"/bin/libCaiss"

        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Environment", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int Environment(uint maxThreadSize,
            CsCaissDefine.CAISS_ALGO_TYPE algoType,
            CsCaissDefine.CAISS_MANAGE_TYPE manageType);

        
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_CreateHandle", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int CreateHandle(ref IntPtr handle);


        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Train", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int Train(IntPtr handle,
            string dataPath,
            uint maxDataSize,
            int normalize,
            uint maxIndexSize = 64,
            float precision = 0.95f,
            uint fastRank = 5,
            uint realRank = 5,
            uint step = 1,
            uint maxEpoch = 5,
            uint showSpan = 1000);


        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Insert", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int Insert(IntPtr handle, float[] node, string label, CsCaissDefine.CAISS_INSERT_TYPE insertType);

        
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Ignore", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int Ignore(IntPtr handle, string label, int isIgnore);
        
        
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Save", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int Save(IntPtr handle, string modelPath);
        

        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_DestroyHandle", CharSet = CharSet.None, ExactSpelling = false)]
        public static extern int DestroyHandle(IntPtr handle);
        
        public static int Init(IntPtr handle,
            CsCaissDefine.CAISS_MODE mode,
            CsCaissDefine.CAISS_DISTANCE_TYPE distanceType,
            uint dim, string modelPath)
        {
            return Init(handle, mode, distanceType, dim, modelPath,null);
        }

        public static string SyncSearch(IntPtr handle,
            string info,
            CsCaissDefine.CAISS_SEARCH_TYPE searchType,
            uint topK,
            int filterEditDistance,
            ref int status)
        {
            if (searchType != CsCaissDefine.CAISS_SEARCH_TYPE.CAISS_LOOP_WORD
                && searchType != CsCaissDefine.CAISS_SEARCH_TYPE.CAISS_SEARCH_WORD)
            {
                status = CsCaissDefine.CAISS_RET_PARAM;
                return "";
            }
            
            var ret = Search(handle, info, searchType, topK, filterEditDistance);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                status = ret;
                return "";
            }
            
            return getResultString(handle, ref status);
        }
        
        public static string SyncSearch(IntPtr handle,
            float[] info,
            CsCaissDefine.CAISS_SEARCH_TYPE searchType,
            uint topK,
            int filterEditDistance,
            ref int status)
        {
            if (searchType != CsCaissDefine.CAISS_SEARCH_TYPE.CAISS_LOOP_QUERY
                && searchType != CsCaissDefine.CAISS_SEARCH_TYPE.CAISS_SEARCH_QUERY)
            {
                status = CsCaissDefine.CAISS_RET_PARAM;
                return "";
            }
            
            var ret = Search(handle, info, searchType, topK, filterEditDistance);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                status = ret;
                return "";
            }

            return getResultString(handle, ref status);
        }

        public static string SyncExecuteSQL(IntPtr handle,
            string sql,
            ref int status)
        {
            var ret = ExecuteSQL(handle, sql);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                status = ret;
                return "";
            }

            return getResultString(handle, ref status);
        }
        
        
        // 以下为内部接口
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Init", CharSet = CharSet.None, ExactSpelling = false)]
        private static extern int Init(IntPtr handle,
            CsCaissDefine.CAISS_MODE mode,
            CsCaissDefine.CAISS_DISTANCE_TYPE distanceType,
            uint dim, string modelPath, CsCaissDefine.CAISS_DIST_FUNC distFunc = null);    // 暂时不支持回调函数
        
        /* 根据单词信息查询 */
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Search", CharSet = CharSet.None, ExactSpelling = false)]
        private static extern int Search(IntPtr handle,
            string info,
            CsCaissDefine.CAISS_SEARCH_TYPE searchType,
            uint topK,
            int filterEditDistance=0,
            CsCaissDefine.CAISS_SEARCH_CALLBACK searchCBFunc = null,
            IntPtr cbParams = new IntPtr()
        );


        /* 根据向量查询信息 */
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_Search", CharSet = CharSet.None, ExactSpelling = false)]
        private static extern int Search(IntPtr handle,
            float[] info,
            CsCaissDefine.CAISS_SEARCH_TYPE searchType,
            uint topK,
            int filterEditDistance = 0,
            CsCaissDefine.CAISS_SEARCH_CALLBACK searchCBFunc = null,
            IntPtr cbParams = new IntPtr()
        );


        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_GetResultSize", CharSet = CharSet.None, ExactSpelling = false)]
        private static extern int GetResultSize(IntPtr handle, ref uint size);
        
        
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_GetResult", CharSet = CharSet.Ansi, ExactSpelling = false)]
        private static extern int GetResult(IntPtr handle, IntPtr result, uint size);
        
        
        [DllImport(CAISS_LIB_PATH, EntryPoint = "CAISS_ExecuteSQL", CharSet = CharSet.None, ExactSpelling = false)]
        private static extern int ExecuteSQL(IntPtr handle,
            string sql,
            CsCaissDefine.CAISS_SQL_CALLBACK sqlCBFunc = null,
            IntPtr cbParams = new IntPtr());
        
        private static string getResultString(IntPtr handle, ref int status)
        {
            // 获取结果信息
            uint size = 0;
            var ret = CsCaiss.GetResultSize(handle, ref size);
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                status = ret;
                return "";
            }

            var resultPtr = Marshal.AllocHGlobal((int)size + 1);    // 申请内存
            Marshal.WriteByte(resultPtr, 0);
            ret = CsCaiss.GetResult(handle, resultPtr, size);
            
            var result = Marshal.PtrToStringAnsi(resultPtr);    // 获取返回的结果字符串
            
            Marshal.FreeHGlobal(resultPtr);    // 释放内存信息
            if (CsCaissDefine.CAISS_RET_OK != ret)
            {
                status = ret;
                return "";
            }

            return result;
        }
    }
}