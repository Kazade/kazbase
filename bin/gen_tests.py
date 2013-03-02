#!/usr/bin/env python
import sys
import re

TEST_CASE_REGEX = r"class\s+(?P<class_name>\S+)\s+:\s+public\s+TestCase"
TEST_FUNC_REGEX = r"void\s+(?P<func_name>test_\S[^\(]+)\(\s*(void)?\s*\)"


INCLUDE_TEMPLATE = "#include \"%(file_path)s\""

REGISTER_TEMPLATE = """
    runner->register_case<%(class_name)s>(
        std::vector<void (%(class_name)s::*)()>({%(members)s}), 
        {%(names)s}
    );
"""

MAIN_TEMPLATE = """

#include <tr1/functional>
#include <tr1/memory>
#include <kglt/kazbase/testing.h>

%(includes)s

int main(int argc, char* argv[]) {
    std::tr1::shared_ptr<TestRunner> runner(new TestRunner());
    
    %(registrations)s
    
    return runner->run();
}


"""


def find_case(line):
    result = re.search(TEST_CASE_REGEX, line)
    if result:
        case = result.group('class_name')
        print("Found case: %s" % case)
        return case
    else:
        return None

def find_test_func(line):
    result = re.search(TEST_FUNC_REGEX, line)
    if result:
        func = result.group('func_name')
        print("Found func: %s" % func)
        return func
    else:
        return None

def main():
    args = sys.argv[1:-1]
    output = sys.argv[-1]
    
    if not args:
        print("Please specify some files to generate tests from")
        return 1

    test_cases = {}
    current_case = None

    for file_path in args:
        content = open(file_path, "r").readlines()
        
        for line in content:
            case = find_case(line)
            if case:
                test_cases.setdefault(case, [])
                current_case = case
                continue

            if current_case:
                #We are searching through a test class
                func = find_test_func(line)
                if func:
                    test_cases[current_case].append(func)
                
    includes = "\n".join([ INCLUDE_TEMPLATE % { 'file_path' : x } for x in args ])
    registrations = []
    for case, funcs in test_cases.items():
        BIND_TEMPLATE = "&%(class_name)s::%(func)s"
        
        members = ", ".join([ BIND_TEMPLATE % { 'class_name' : case, 'func' : x } for x in funcs ])
        names = ", ".join([ '"%s::%s"' % (case, x) for x in funcs ])
        
        registrations.append(REGISTER_TEMPLATE % { 'class_name' : case, 'members' : members, 'names' : names })
    
    registrations = "\n".join(registrations)
    
    final = MAIN_TEMPLATE % {
        'registrations' : registrations,
        'includes' : includes
    }
    
    open(output, "w").write(final)
    
    return 0

if __name__ == '__main__':
    sys.exit(main())
