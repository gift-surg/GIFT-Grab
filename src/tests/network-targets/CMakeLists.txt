SET(NAME_TEST Test_NetworkTargets)
ADD_TEST(NAME ${NAME_TEST} COMMAND py.test test_network_targets.py)
LIST(APPEND TESTS_LIST ${NAME_TEST})
