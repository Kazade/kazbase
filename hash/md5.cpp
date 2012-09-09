#include <iostream>
#include <cstdio>
#include <cassert>
#include "md5.h"

namespace hashlib {

MD5::MD5():
    ctx_(std::tr1::shared_ptr<MD5_CTX>(new MD5_CTX())) {
    MD5_Init(ctx_.get());
}

MD5::MD5(const std::string& data):
    ctx_(std::tr1::shared_ptr<MD5_CTX>(new MD5_CTX())) {

    MD5_Init(ctx_.get());
    update(data);
}

void MD5::update(const std::string& data) {
    assert(!data.empty());

    std::cout << data << std::endl;
    MD5_Update(ctx_.get(), (void*) data.c_str(), data.length());
}

std::string MD5::hex_digest() {
    unsigned char result[16] = {0};

    MD5_Final(result, ctx_.get());

    std::string final;
    char szTemp[16] = {0};
    for(int i = 0; i < 16; ++i) {
        sprintf(szTemp, "%02X", result[i]);
        final += std::string(szTemp);
    }

    std::cout << final << std::endl;
    return final;
}

}

