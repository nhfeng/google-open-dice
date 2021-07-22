// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#ifndef DICE_OPS_H_
#define DICE_OPS_H_

#include <dice/dice.h>

// These are the set of functions that implement various operations that the
// main DICE functions depend on. They are provided as part of an integration
// and resolved at link time.

#ifdef __cplusplus
extern "C" {
#endif

// An implementation of SHA-512, or an alternative hash. Hashes |input_size|
// bytes of |input| and populates |output| on success.
DiceResult DiceHash(void* context, const uint8_t* input, size_t input_size,
                    uint8_t output[DICE_HASH_SIZE]);

// An implementation of HKDF-SHA512, or an alternative KDF. Derives |length|
// bytes from |ikm|, |salt|, and |info| and populates |output| on success.
// |Output| must point to a buffer of at least |length| bytes.
DiceResult DiceKdf(void* context, size_t length, const uint8_t* ikm,
                   size_t ikm_size, const uint8_t* salt, size_t salt_size,
                   const uint8_t* info, size_t info_size, uint8_t* output);

// Deterministically generates a public and private key pair from |seed|.
// Since this is deterministic, |seed| is as sensitive as a private key and can
// be used directly as the private key. The |private_key| may use an
// implementation defined format so may only be passed to the |sign| operation.
DiceResult DiceKeypairFromSeed(void* context,
                               const uint8_t seed[DICE_PRIVATE_KEY_SEED_SIZE],
                               uint8_t public_key[DICE_PUBLIC_KEY_MAX_SIZE],
                               size_t* public_key_size,
                               uint8_t private_key[DICE_PRIVATE_KEY_MAX_SIZE],
                               size_t* private_key_size);

// Calculates a signature of |message_size| bytes from |message| using
// |private_key|. |private_key| was generated by |keypair_from_seed| to allow
// an implementation to use their own private key format. |signature| points to
// |signature_size| bytes into which the calculated signature is written.  If
// |signature_size| differs from the implementation's signature size,
// kDiceResultPlatformError is returned.
DiceResult DiceSign(void* context, const uint8_t* message, size_t message_size,
                    const uint8_t* private_key, size_t private_key_size,
                    size_t signature_size, uint8_t* signature);

// Verifies, using |public_key|, that |signature| covers |message_size| bytes
// from |message|.
DiceResult DiceVerify(void* context, const uint8_t* message,
                      size_t message_size, const uint8_t* signature,
                      size_t signature_size, const uint8_t* public_key,
                      size_t public_key_size);

// Generates an X.509 certificate, or an alternative certificate format, from
// the given |subject_private_key_seed| and |input_values|, and signed by
// |authority_private_key_seed|. The subject private key seed is supplied here
// so the implementation can choose between asymmetric mechanisms, for example
// ECDSA vs Ed25519.
DiceResult DiceGenerateCertificate(
    void* context,
    const uint8_t subject_private_key_seed[DICE_PRIVATE_KEY_SEED_SIZE],
    const uint8_t authority_private_key_seed[DICE_PRIVATE_KEY_SEED_SIZE],
    const DiceInputValues* input_values, size_t certificate_buffer_size,
    uint8_t* certificate, size_t* certificate_actual_size);

// Securely clears |size| bytes at |address|. This project contains a basic
// implementation. OPENSSL_cleanse from boringssl, SecureZeroMemory from
// Windows and memset_s from C11 could also be used as an implementation but a
// particular target platform or toolchain may have a better implementation
// available that can be plugged in here. Care may be needed to ensure sensitive
// data does not leak due to features such as caches.
void DiceClearMemory(void* context, size_t size, void* address);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // DICE_OPS_H_