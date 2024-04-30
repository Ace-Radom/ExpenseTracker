import configparser

def split_data_type_and_default_value( data: str ) -> tuple[str,str]:
    return data[:data.find( "," )] , data[data.find( "," ) + 1:]

def update_config_declaration( item: str , data_type: str ) -> str:
    return f"            static { data_type } { item.upper() };\n"

def update_config_default_settings( item: str , default_value: str ) -> str:
    return f"    this -> { item.upper() } = { default_value };\n"

def update_config_definition( item: str , data_type: str ) -> str:
    return f"{ data_type } core::config::{ item.upper() };\n"

def update_config_read_section( section: str ) -> str:
    return f"    if ( !ini.has( \"{ section }\" ) )\n    {{\n        throw config_exception( ERR_CONFIG_CFGFILE_FORMAT_ERROR , \"config file doesn't have section \\\"{ section }\\\"\" );\n    }}\n"

def update_config_read_key( section: str , item: str , data_type: str , default_value: str ) -> str:
    config_read_key = ""
    config_read_key += f"    if ( !ini[\"{ section }\"].has( \"{ item }\" ) )\n"
    config_read_key += "    {\n"
    config_read_key += f"        throw config_exception( ERR_CONFIG_CFGFILE_FORMAT_ERROR , \"config file section \\\"{ section }\\\" doesn't have node \\\"{ item }\\\"\" );\n"
    config_read_key += "    }\n"
    if data_type == "const char*":
        config_read_key += f"    this -> { item.upper() } = ini[\"{ section }\"][\"{ item }\"];\n"
    elif data_type == "bool":
        config_read_key += f"    this -> { item.upper() } = ini[\"{ section }\"][\"{ item }\"] != \"0\";\n"
    elif data_type == "int":
        config_read_key += "    try {\n"
        config_read_key += f"        this -> { item.upper() } = std::stoi( ini[\"{ section }\"][\"{ item }\"] );\n"
        config_read_key += "    }\n"
        config_read_key += "    catch ( ... )\n"
        config_read_key += "    {\n"
        config_read_key += f"        this -> { item.upper() } = { default_value };\n"
        config_read_key += "    }\n"
    else:
        raise Exception( "unrecognized data type" )
    return config_read_key

def update_config_create( section: str , item: str , data_type: str , default_value: str ) -> str:
    if data_type == "bool":
        if default_value == "true":
            default_value = "1"
        elif default_value == "false":
            default_value = "0"
        else:
            raise Exception( "unrecognized bool default value" )
    return f"    ini[\"{ section }\"][\"{ item }\"] = \"{ default_value }\";\n"

def main():
    cp = configparser.ConfigParser()
    cp.read( "./data/update_config.cfg" , encoding = 'utf-8' )
    target_header = cp.get( "target" , "header" )
    target_source = cp.get( "target" , "source" )
    source_header = cp.get( "source" , "header" )
    source_source = cp.get( "source" , "source" )

    i_config_declaration = ""
    i_config_default_settings = ""
    i_config_definition = ""
    i_config_read = ""
    i_config_create = ""

    print( "Updating config:" )
    print( f" - { source_header } -> { target_header }" )
    print( f" - { source_source } -> { target_source }" )

    section_list = []
    print( "Found following config sections:" )
    for section in cp:
        if section != "target" and section != "source" and section != "DEFAULT":
            section_list.append( section )
            print( f" - { section }" )

    for section in section_list:
        print( f"Entering section: { section }..." )
        i_config_read += update_config_read_section( section )
        for item in cp.items( section ):
            print( f"Parsing item: { item }..." )
            data_type , default_value = split_data_type_and_default_value( item[1] )
            i_config_declaration += update_config_declaration( item[0] , data_type )
            i_config_default_settings += update_config_default_settings( item[0] , default_value )
            i_config_definition += update_config_definition( item[0] , data_type )
            i_config_read += update_config_read_key( section , item[0] , data_type , default_value )
            i_config_create += update_config_create( section , item[0] , data_type , default_value )

    with open( source_header , 'r' ) as rFile:
        header_code = rFile.read()

    with open( source_source , 'r' ) as rFile:
        source_code = rFile.read()

    header_code = header_code.replace( "@INSERT_CONFIG_DECLARATION@" , i_config_declaration[:-1] )
    source_code = source_code.replace( "@INSERT_CONFIG_DEFAULT_SETTINGS@" , i_config_default_settings[:-1] )
    source_code = source_code.replace( "@INSERT_CONFIG_DEFINITION@" , i_config_definition[:-1] )
    source_code = source_code.replace( "@INSERT_CONFIG_READ@" , i_config_read[:-1] )
    source_code = source_code.replace( "@INSERT_CONFIG_CREATE@" , i_config_create[:-1] )

    with open( target_header , 'w' ) as wFile:
        wFile.write( header_code )

    with open( target_source , 'w' ) as wFile:
        wFile.write( source_code )

if __name__ == "__main__":
    main()
