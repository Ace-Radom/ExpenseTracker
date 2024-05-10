import configparser
import os

def update_tests_build( name: str ) -> str:
    test_build = ""
    test_build += f"add_executable({ name } ${{CMAKE_CURRENT_SOURCE_DIR}}/{ name }.cpp)\n"
    test_build += f"target_include_directories({ name } PUBLIC ${{ETSCRIPT_INCLUDE_DIR}})\n"
    test_build += f"target_link_libraries({ name } etscript)\n"
    test_build += f"# { name }\n"
    return test_build

def update_tests_declaration( name: str ) -> str:
    return f"add_test(NAME etscript_{ name } COMMAND { name })\n"

def main():
    cp = configparser.ConfigParser()
    cp.read( "./data/update_tests.cfg" , encoding = 'utf-8' )
    target_cmakelists = cp.get( "target" , "cmakelists" )
    source_cmakelists = cp.get( "source" , "cmakelists" )
    source_testdir = cp.get( "source" , "testdir" )

    i_tests_build = ""
    i_tests_declaration = ""

    test_files = []
    print( "Found following tests:" )
    for root , ds , fs in os.walk( source_testdir ):
        for f in fs:
            if f.endswith( ".cpp" ):
                test_files.append( os.path.splitext( os.path.basename( f ) )[0] )
                print( f" - { test_files[-1] }" )

    for test_file in test_files:
        i_tests_build += update_tests_build( test_file )
        i_tests_declaration += update_tests_declaration( test_file )

    with open( source_cmakelists , 'r' ) as rFile:
        cmakelists_code = rFile.read()

    cmakelists_code = cmakelists_code.replace( "@INSERT_TESTS_BUILD@" , i_tests_build[:-1] )
    cmakelists_code = cmakelists_code.replace( "@INSERT_TESTS_DECLARATION@" , i_tests_declaration[:-1] )

    with open( target_cmakelists , 'w' ) as wFile:
        wFile.write( cmakelists_code )

if __name__ == "__main__":
    main()
