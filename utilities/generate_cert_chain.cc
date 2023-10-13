//  Copyright (c) 2021-22, VMware Inc, and the Certifier Authors.  All rights
//  reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gflags/gflags.h>
#include "support.h"

using namespace certifier::utilities;

DEFINE_bool(print_all, false, "verbose");

// "generate" or "print" are the other option
DEFINE_string(operation, "", "generate or print cert chain ");

DEFINE_string(root_key_name, "policyKey", "key name");
DEFINE_string(key_type, Enc_method_rsa_2048_private, "key type");
DEFINE_string(authority_name,
              "rootAuthority",
              "root authority name");
DEFINE_string(output_file,
              "output.bin",
              "output file");
DEFINE_int32(num_intermediate, 0, "number of intermediate certs");
DEFINE_string(input_file,
              "input.bin",
              "input file");

bool generate_key(const string &type, const string &name, key_message *k) {

  if (type == Enc_method_rsa_1024) {
    RSA *r = RSA_new();
    if (!generate_new_rsa_key(1024, r)) {
      printf("Can't generate rsa key\n");
      return false;
    }
    if (!RSA_to_key(r, k)) {
      printf("Can't convert rsa key to key\n");
      return false;
    }
    k->set_key_type(Enc_method_rsa_1024_private);
  } else if (type == Enc_method_rsa_2048) {
    RSA *r = RSA_new();
    if (!generate_new_rsa_key(2048, r)) {
      printf("Can't generate rsa key\n");
      return false;
    }
    if (!RSA_to_key(r, k)) {
      printf("Can't convert rsa key to key\n");
      return false;
    }
    k->set_key_type(Enc_method_rsa_2048_private);
  } else if (type == Enc_method_rsa_3072) {
    RSA *r = RSA_new();
    if (!generate_new_rsa_key(3072, r)) {
      printf("Can't generate rsa key\n");
      return false;
    }
    if (!RSA_to_key(r, k)) {
      printf("Can't convert rsa key to key\n");
      return false;
    }
    k->set_key_type(Enc_method_rsa_3072_private);
  } else if (type == Enc_method_rsa_4096) {
    RSA *r = RSA_new();
    if (!generate_new_rsa_key(4096, r)) {
      printf("Can't generate rsa key\n");
      return false;
    }
    if (!RSA_to_key(r, k)) {
      printf("Can't convert rsa key to key\n");
      return false;
    }
    k->set_key_type(Enc_method_rsa_4096_private);
  } else if (type == Enc_method_ecc_384) {
    EC_KEY *ec = generate_new_ecc_key(384);
    if (ec == nullptr) {
      printf("Can't generate ecc key\n");
      return false;
    }
    if (!ECC_to_key(ec, k)) {
      printf("Can't convert ecc key to key\n");
      return false;
    }
    k->set_key_type(Enc_method_ecc_384_private);
  } else {
    printf("Unknown key type\n");
    return false;
  }

  k->set_key_name(name);
  k->set_key_format("vse-key");

  return true;
}

bool generate_chain(const string& root_key_name,
                    const string& key_type,
                    const string& authority_name,
                    int num_intermediate,
                    full_cert_chain* chain) {

  // generate root
  key_message private_root_key;
  if (!generate_key(key_type, root_key_name, &private_root_key)) {
    printf("%s:%d: %s() generate root key failed\n", __FILE__, __LINE__, __func__);
    return false;
  }
  key_message public_root_key;
  if (!private_key_to_public_key(private_root_key, &public_root_key)) {
    printf("%s:%d: %s() private root key to public failed\n", __FILE__, __LINE__, __func__);
    return false;
  }

  X509* x509_root_cert = X509_new();
  uint64_t sn = 1;
  if (!produce_artifact(private_root_key,
                        (string&)root_key_name,
                        (string&)authority_name,
                        public_root_key,
                        (string&)root_key_name,
                        (string&)authority_name,
                        sn,
                        365.0 * 86400,
                        x509_root_cert,
                        true,
                        false)) {
    printf("%s:%d: %s() Can't produce root cert\n", __FILE__, __LINE__, __func__);
    return false;
  }
  printf("\nRoot cert:\n");
  X509_print_fp(stdout, x509_root_cert);
  printf("\n");
  full_cert_chain_entry* ent = chain->add_list();

  // add root to first entry
  string root_der;
  if (!x509_to_asn1(x509_root_cert, &root_der)) {
    printf("%s:%d: %s() Can't convert root cert to der\n", __FILE__, __LINE__, __func__);
    return false;
  }
  ent->mutable_subject_key()->CopyFrom(public_root_key);
  ent->mutable_signer_key()->CopyFrom(private_root_key);
  ent->set_der_cert(root_der);

  // generate intermediates
  key_message private_intermediates[num_intermediate + 1];
  key_message public_intermediates[num_intermediate + 1];

  private_intermediates[0].CopyFrom(private_root_key);
  public_intermediates[0].CopyFrom(public_root_key);
  for (int i = 1; i <= num_intermediate; i++) {
  }

  // generate final
  key_message final_private_key;
  key_message final_public_key;

  return true;
}


int main(int an, char **av) {
  string usage("Utility to generate cert chains");
  gflags::SetUsageMessage(usage);
  gflags::ParseCommandLineFlags(&an, &av, true);

  if (FLAGS_operation == "") {
    printf("%s: %s\n", av[0], usage.c_str());
    printf("\n%s --operation=<generate print> "
           "--num_intermediate=nn "
           "--input_file=<file.bin> "
           "--output_file=<file.bin> "
           "--key_type=<rsa-4096 rsa-2048 ecc-256 ecc-384\n",
           av[0]);

    return 0;
  } else if (FLAGS_operation == "print") {

    string str;
    full_cert_chain chain; // list

    // read file
    if (!read_file_into_string(FLAGS_input_file, &str)) {
      printf("%s:%d: %s() read_file_into_string failed\n", __FILE__, __LINE__, __func__);
      return 1;
    }

    // deserialize
    if (!chain.ParseFromString(str)) {
      printf("%s:%d: %s() chain.ParseFromString failed\n", __FILE__, __LINE__, __func__);
      return 1;
    }

    // print them
    for (int i = 0; i < chain.list_size(); i++) {
      printf("\n Cert %d:\n", i);
      const full_cert_chain_entry ent = chain.list(i);
      const string & ser_cert = ent.der_cert();
      X509* cert = X509_new();
      if (!asn1_to_x509(ser_cert, cert)) {
        printf("%s:%d: %s() asn1_to_x509 failed\n", __FILE__, __LINE__, __func__);
        continue;
      }
      X509_print_fp(stdout, cert);
      X509_free(cert);
      printf("\n");
    }

    return 0;
  } else if (FLAGS_operation == "generate") {
    full_cert_chain chain; // list
    if (!generate_chain(FLAGS_root_key_name,
                        FLAGS_key_type,
                        FLAGS_authority_name,
                        FLAGS_num_intermediate,
                        &chain)) {
      printf("%s:%d: %s() failed\n", __FILE__, __LINE__, __func__);
      return 1;
    }
    // serialize and save
    return 0;
  } else {
    printf("Unknown operation\n");
    return 1;
  }

  return 0;
}