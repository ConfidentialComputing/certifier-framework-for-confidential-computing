// swigpytest.i- SWIG interface to generate Python bindings
// *****************************************************************************
// SWIG interfaces to test out small samples of C++ code and headers
// to understand and build pytests to exercise different interfaces.
//
// The tests verifying these interfaces are in test_libswigpytest.py
// *****************************************************************************

%module swigpytest
%include "std_string.i"

// Test: test_secure_authenticated_channel_lib()
//       test_secure_authenticated_channel_default()
%apply string * INPUT  { string& role };            // secure_authenticated_channel() constructor

// You don't really need this rule as 'const string &arg' is passed-in by default as input string.
// Test: test_secure_authenticated_channel_init_client_ssl_default()
//       test_secure_authenticated_channel_init_client_ssl_simple_app()
// %apply string * INPUT  { const string &asn1_root_cert };  // secure_authenticated_channel().init_client_ssl()

%{
#include "swigpytest.h"
%}

%include "swigpytest.h"
