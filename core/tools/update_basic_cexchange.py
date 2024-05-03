import configparser

def update_currency_cases( code: str , name: str ) -> str:
    return f"        case { code }: return {{ { code } , \"{ code }\" , \"{ name }\" }}; break;\n"

def main():
    cp = configparser.ConfigParser()
    cp.read( "./data/update_basic_cexchange.cfg" , encoding = 'utf-8' )
    target_source = cp.get( "target" , "source" )
    source_source = cp.get( "source" , "source" )
    source_data = cp.get( "source" , "data" )

    i_currency_cases = ""
    
    print( "Updating basic_cexchange:" )
    print( f" - { source_source } -> { target_source }" )

    with open( source_data , 'r' , encoding = 'utf-8' ) as rFile:
        while True:
            this_line = rFile.readline()
            if not this_line:
                break
            this_line = this_line.strip()
            currency_code = this_line[:3]

            tab_pos = 0
            for i in range( 3 ):
                tab_pos = this_line.find( '\t' , tab_pos + 1 )
                if tab_pos == -1:
                    raise Exception( "third tab not found" )
            currency_name = this_line[tab_pos+1:this_line.find( '\t' , tab_pos + 1 )]

            i_currency_cases += update_currency_cases( currency_code , currency_name )

    with open( source_source , 'r' , encoding = 'utf-8' ) as rFile:
        source_code = rFile.read()

    source_code = source_code.replace( "@INSERT_CURRENCY_CASES@" , i_currency_cases[:-1] )

    with open( target_source , 'w' , encoding = 'utf-8' ) as wFile:
        wFile.write( source_code )

if __name__ == "__main__":
    main()
