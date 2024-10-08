// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4; tab-width: 8; -*-
//
//  
//
// Copyright (C) 2009         Dirk Eddelbuettel 
// Copyright (C) 2010 - 2011  Dirk Eddelbuettel and Romain Francois
//
// GPL'ed 

#include <RInside.h> // for the embedded R via RInside
#include <iostream>

int main(int argc, char *argv[]) {

    std::string src = argv[1];
    std::string word = argv[2];
    std::string iteration = argv[3];


    RInside R(argc, argv); // create an embedded R instance

        //String Declarations:
        R["plot_funcs_src_path"] = "./Animation/animate_plots.R";
        R["data_src_path"] = src;
        R["word"] = word;
        R["iteration"] = iteration;

        //R Code:
        std::string RCode =
            "source(plot_funcs_src_path); "
            "p <- plot_sens_on_iter(data_src_path, word, iteration); ";

        R.parseEvalQ(RCode);

    exit(0);
}