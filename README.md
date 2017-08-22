# CMLManager

This is an utility that allows for a fast (re)building of all CMakeLists within a directory tree

Usage :
    -Either in command line : CMLManager [pathToDir] [-noConfirm] [-recursive]
        -noConfirm : Pass this argument if you don't want to have to confirm the building of CMakeLists
        -recursive : Is the soft building all CMakeLists for the whole tree, or just given directory

    -Or you can have within app's directory a folder Option containing option.txt :
        -CMLManager
            -Options
                -option.txt
            -CMLManager.bin

    in option.txt, you just put as many paths as you want, one by line
        path1/path1
        path2
        path3/3/3
        etc

    Then you just execute CMLManager with no args, it'll read option.txt