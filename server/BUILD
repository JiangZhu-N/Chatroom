package(default_visibility = ["//visibility:public"])

cc_library(
    name = "net_wrap",
    srcs = ["net_wrap.cc"],
    hdrs = ["net_wrap.h"],
    linkopts =["-lpthread","-ljsoncpp"],
)

cc_library(
    name = "serverClass",
    srcs = ["serverClass.cc"],
    hdrs = ["serverClass.h"],
    deps = ["net_wrap"],
)

cc_binary(
    name = "server",
    srcs = ["privateRoom.cc",
        "friends_menu.cc",
        "main_menu.cc",
        "sign.cc",
        "server.cc",
        "group_menu.cc",
        "publicRoom.cc",
        "file.cc"],
    deps = ["net_wrap","serverClass"],
    linkopts =["-lleveldb"],
)