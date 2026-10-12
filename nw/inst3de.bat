@echo off
IF '%1==' goto Usage
copy include\graph.h %1
copy include\patterns.h %1
copy include\view.h %1
copy include\objmap.h %1
copy include\dynamic.h %1
copy include\keyframe.h %1
copy include\namerefs.h %1
copy include\land.h %1
copy obj\3d\3ded.lib %1

:Usage
echo Usage: inst3de dir
echo E.g: inst3de d:\engine
echo      inst3de d:.
:End
