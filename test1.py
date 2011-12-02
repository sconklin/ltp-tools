import sys
import JUnit_api as api

def test():

    properties = {}
    testcases = [] # each testcase in this list is a dictionary

    tss = api.testsuites()

    # Add a test testsuite
    properties['prop1'] = 'foo'
    properties['prop2'] = 'bar'

    ts = api.testsuite(name='My Test Name')
    props = api.propertiesType()

    for key, val in properties.items():
        tp = api.propertyType(key, val)
        props.add_property(tp)
    ts.set_properties(props)

    #ts.name = "My Test Name"
    ts.failures = 1
    ts.errors = 1
    #ts.timestamp = None
    #ts.hostname = None
    ts.tests = 2
    ts.time = 456

    # now add test cases
    tc = api.testcaseType()
    tc.name = 'my test case one'
    tc.classname = 'ltp' # Normally associated with java classes in JUnit
    tc.time = 23
    error = api.errorType(message='Error Message', type_ = 'A', valueOf_ = "This is the test output string ONE")
    tc.error = error
    ts.add_testcase(tc)

    # now add test cases
    tc = api.testcaseType()
    tc.name = 'my test case Two'
    tc.classname = 'ltp' # Normally associated with java classes in JUnit
    tc.time = 98
    error = api.failureType(message='Failure Message', type_ = 'B', valueOf_ = "This is the test output string TWO")
    # TODO WHAT IS type?
    tc.error = error
    ts.add_testcase(tc)

    ts.system_out = 'This is stdout'
    ts.system_err = 'This is stderr'

    tss.add_testsuite(ts)
    tss.export(sys.stdout, 0)

test()
