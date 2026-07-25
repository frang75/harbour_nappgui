nap_library(hboffice "officesdk" NO NRC_NONE)
target_include_directories(hboffice PUBLIC "../officesdk/include")  # For officesdk headers
target_include_directories(hboffice PUBLIC "..")                    # For hboffice.ch
target_include_directories(hboffice PUBLIC "../../../include")      # For Harbour headers
