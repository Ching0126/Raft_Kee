# CMake generated Testfile for 
# Source directory: /root/Raft_Kee
# Build directory: /root/Raft_Kee/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[state_machine_test]=] "/root/Raft_Kee/build/state_machine_test")
set_tests_properties([=[state_machine_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/root/Raft_Kee/CMakeLists.txt;28;add_test;/root/Raft_Kee/CMakeLists.txt;0;")
add_test([=[local_log_test]=] "/root/Raft_Kee/build/local_log_test")
set_tests_properties([=[local_log_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/root/Raft_Kee/CMakeLists.txt;32;add_test;/root/Raft_Kee/CMakeLists.txt;0;")
add_test([=[rpc_dispatcher_test]=] "/root/Raft_Kee/build/rpc_dispatcher_test")
set_tests_properties([=[rpc_dispatcher_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/root/Raft_Kee/CMakeLists.txt;76;add_test;/root/Raft_Kee/CMakeLists.txt;0;")
