#ifndef _VERSION_H_
#define _VERSION_H_
/* 
========================================
 TLOL版本相关说明见文档
 svn://10.6.34.93/MOBILERPG_RES/Branches/Version/Main/Doc/公共/移动天龙版本号命名规范 V1.0.docx

 服务器版本号格式：A.B.C.D
 ABCD的说明如下： 
 A-游戏大版本号（GAMEVERSION），B-代码版本号（PROGRAMVERSION），C-公共资源版本号（PublicResourceVersion）,D-服务器内部版本号（SERVERINTERNALVERSION）.
 其中ABD写在代码中，即下面三个，C配置在公共PublicConfig.txt文件中
========================================
*/

//游戏大版本号(A)
#define GAMEVERSION 0
//代码版本号(B)
#define PROGRAMVERSION 507
//服务器内部版本号(D)
#define SERVERINTERNALVERSION 0
//数据库版本号
#define DBVERSION 503

#endif