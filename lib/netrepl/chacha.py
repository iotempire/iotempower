# """
#     chacha.py
#
#     An implementation of ChaCha in about 130 operative lines
#     of 100% pure Python code.
#
#     Copyright (c) 2009-2011 by Larry Bugbee, Kent, WA
#     ALL RIGHTS RESERVED.
#
#     chacha.py IS EXPERIMENTAL SOFTWARE FOR EDUCATIONAL
#     PURPOSES ONLY.  IT IS MADE AVAILABLE "AS-IS" WITHOUT
#     WARRANTY OR GUARANTEE OF ANY KIND.  USE SIGNIFIES
#     ACCEPTANCE OF ALL RISK.
#
#     To make your learning and experimentation less cumbersome,
#     chacha.py is free for any use.
#
#     This implementation is intended for Python 2.x.
#
#     Larry Bugbee
#     May 2009     (Salsa20)
#     August 2009  (ChaCha)
#     rev June 2010
#     rev March 2011  - tweaked _quarterround() to get 20-30% speed gain
#
#     Modifications by ulno (http://ulnol.net) starting on 2017-09-20.
#     Taken from: http://www.seanet.com/~bugbee/crypto/chacha/chacha.py
#
#     Intended to be used as stream cipher for repl in micropython.
#
#     This version is optimized for micropython.
#
# """

try: # micropython
    from micropython import const
    import ustruct as struct
    import uos as os
    import urandom as random
except: # normal python
    def const(a): return a
    try:
        import struct
        import os
        import random
    except:
        pass
import errno
import time
from cpointers import p32 as ptr32

#-----------------------------------------------------------------------

class ChaCha(object):
    # """
    #     ChaCha is an improved variant of Salsa20.
    #
    #     Salsa20 was submitted to eSTREAM, an EU stream cipher
    #     competition.  Salsa20 was originally defined to be 20
    #     rounds.  Reduced round versions, Salsa20/8 (8 rounds) and
    #     Salsa20/12 (12 rounds), were later submitted.  Salsa20/12
    #     was chosen as one of the winners and 12 rounds was deemed
    #     the "right blend" of security and efficiency.  Salsa20
    #     is about 3x-4x faster than AES-128.
    #
    #     Both ChaCha and Salsa20 accept a 128-bit or a 256-bit key
    #     and a 64-bit IV to set up an initial 64-byte state.  For
    #     each 64-bytes of data, the state gets scrambled and XORed
    #     with the previous state.  This new state is then XORed
    #     with the input data to produce the output.  Both being
    #     stream ciphers, their encryption and decryption functions
    #     are identical.
    #
    #     While Salsa20's diffusion properties are very good, some
    #     claimed the IV/keystream correlation was too strong for
    #     comfort.  To satisfy, another variant called XSalsa20
    #     implements a 128-bit IV.  For the record, EU eSTREAM team
    #     did select Salsa20/12 as a solid cipher providing 128-bit
    #     security.
    #
    #     ChaCha is a minor tweak of Salsa20 that significantly
    #     improves its diffusion per round.  ChaCha is more secure
    #     than Salsa20 and 8 rounds of ChaCha, aka ChaCha8, provides
    #     128-bit security.  (FWIW, I have not seen any calls for a
    #     128-bit IV version of ChaCha or XChaCha.)
    #
    #     Another benefit is that ChaCha8 is about 5-8% faster than
    #     Salsa20/8 on most 32- and 64-bit PPC and Intel processors.
    #     SIMD machines should see even more improvement.
    #
    #     Sample usage:
    #       from chacha import ChaCha
    #
    #       cc8 = ChaCha(key, iv)
    #       ciphertext = cc8.encrypt(plaintext)
    #
    #       cc8 = ChaCha(key, iv)
    #       plaintext = cc8.decrypt(ciphertext)
    #
    #     Remember, the purpose of this program is educational; it
    #     is NOT a secure implementation nor is a pure Python version
    #     going to be fast.  Encrypting large data will be less than
    #     satisfying.  Also, no effort is made to protect the key or
    #     wipe critical memory after use.
    #
    #     For more information about Daniel Bernstein's ChaCha
    #     algorithm, please see http://cr.yp.to/chacha.html
    #
    #     All we need now is a keystream AND authentication in the
    #     same pass.
    #
    #     Larry Bugbee
    #     May 2009     (Salsa20)
    #     August 2009  (ChaCha)
    #     rev June 2010
    # """

#    ROUNDS = 8                         # ...10, 12, 20?
    ROUNDS = const(12)                         # ...10, 12, 20? [ulno] playing it safer with 12

    def __init__(self, key, iv, rounds=ROUNDS):
        # """ Both key and iv are byte strings.  The key must be
        #     exactly 16 or 32 bytes, 128 or 256 bits respectively.
        #     The iv must be exactly 8 bytes (64 bits) and MUST
        #     never be reused with the same key.
        #
        #     The default number of rounds is 8.
        #
        #     If you have several encryptions/decryptions that use
        #     the same key, you may reuse the same instance and
        #     simply call iv_setup() to set the new iv.  The previous
        #     key and the new iv will establish a new state.
        #
        # """
        self._key_setup(bytearray(key))
        self.iv_setup(bytearray(iv))
        self.rounds = rounds

    def _key_setup(self, key):
        # """ key is converted to a list of 4-byte unsigned integers
        #     (32 bits).
        #
        #     Calling this routine with a key value effectively resets
        #     the context/instance.  Be sure to set the iv as well.
        # """
        if len(key) not in [16, 32]:
            raise Exception('key must be either 16 or 32 bytes')
        key=bytearray(key)
        self.key_state = bytearray(64)
        key_state = ptr32(self.key_state)

        if len(key) == 16:
            k = ptr32(key) # access as 4x4

            tau = ptr32(bytearray(b"expand 16-byte k"))
            key_state[0]  = tau[0]
            key_state[1]  = tau[1]
            key_state[2]  = tau[2]
            key_state[3]  = tau[3]
            key_state[4]  = k[0]
            key_state[5]  = k[1]
            key_state[6]  = k[2]
            key_state[7]  = k[3]
            key_state[8]  = k[0]
            key_state[9]  = k[1]
            key_state[10] = k[2]
            key_state[11] = k[3]
            # 12 and 13 are reserved for the counter
            # 14 and 15 are reserved for the IV

        elif len(key) == 32:
            k = ptr32(key) # access as 8x4
            sigma = ptr32(bytearray(b"expand 32-byte k"))
            key_state[0]  = sigma[0]
            key_state[1]  = sigma[1]
            key_state[2]  = sigma[2]
            key_state[3]  = sigma[3]
            key_state[4]  = k[0]
            key_state[5]  = k[1]
            key_state[6]  = k[2]
            key_state[7]  = k[3]
            key_state[8]  = k[4]
            key_state[9]  = k[5]
            key_state[10] = k[6]
            key_state[11] = k[7]
            # 12 and 13 are reserved for the counter
            # 14 and 15 are reserved for the IV


    def iv_setup(self, iv):
        # """ self.state and other working structures are lists of
        #     4-byte unsigned integers (32 bits).
        #
        #     The iv is not a secret but it should never be reused
        #     with the same key value.  Use date, time or some other
        #     counter to be sure the iv is different each time, and
        #     be sure to communicate the IV to the receiving party.
        #     Prepending 8 bytes of iv to the ciphertext is the usual
        #     way to do this.
        #
        #     Just as setting a new key value effectively resets the
        #     context, setting the iv also resets the context with
        #     the last key value entered.
        # """
        if len(iv) != 8:
            raise Exception('iv must be 8 bytes')
        v = ptr32(iv)
        self.state = bytearray(self.key_state)
        self.scramble_buf = bytearray(self.key_state)
        self.scramble_pos = 64 # all used up to trigger new generation
        iv_state = ptr32(self.state)
        iv_state[12] = 0
        iv_state[13] = 0
        iv_state[14] = v[0]
        iv_state[15] = v[1]

    def encrypt(self, data, length=None):
        # """ Encrypt a chunk of data.  datain and the returned value
        #     are byte strings.
        #
        #     If large data is submitted to this routine in chunks,
        #     the chunk size MUST be an exact multiple of 64 bytes.
        #     Only the final chunk may be less than an even multiple.
        #     (This function does not "save" any uneven, left-over
        #     data for concatenation to the front of the next chunk.)
        #
        #     The amount of available memory imposes a poorly defined
        #     limit on the amount of data this routine can process.
        #     Typically 10's and 100's of KB are available but then,
        #     perhaps not.  This routine is intended for educational
        #     purposes so application developers should take heed.
        #
        # As we use xor, en-/decryption can be done in place
        #
        # """
        if length is None: data_size=len(data)
        else: data_size=length
        self._scramble_xor(data, data_size) # XOR the stream

    decrypt = encrypt # same -> we are using xor against the evolving scramble-buffer

    def _chacha_scramble(self):     # 64 bytes in
        # """ self.state and other working structures are lists of
        #     4-byte unsigned integers (32 bits).
        #
        #     output must be converted to bytestring before return.
        # """

        # make a copy of state
        s = ptr32(self.state)
        x = ptr32(self.scramble_buf)
        for i in range(16): x[i] = s[i]

        for i in range(0, self.rounds, 2):
            # two rounds per iteration
            self._quarterround(x, 0, 4, 8,12)
            self._quarterround(x, 1, 5, 9,13)
            self._quarterround(x, 2, 6,10,14)
            self._quarterround(x, 3, 7,11,15)
            
            self._quarterround(x, 0, 5,10,15)
            self._quarterround(x, 1, 6,11,12)
            self._quarterround(x, 2, 7, 8,13)
            self._quarterround(x, 3, 4, 9,14)
            
        for i in range(16):
            x[i] = (x[i] + s[i]) & 0xffffffff
        self.scramble_pos = 0

        # increment the iv.  In this case we increment words
        # 12 and 13 in little endian order.  This will work
        # nicely for data up to 2^70 bytes (1,099,511,627,776GB)
        # in length.  After that it is the user's responsibility
        # to generate a new nonce/iv.
        iv = ptr32(self.state)
        iv[12] = (iv[12] + 1) & 0xffffffff
        if iv[12] == 0:  # if overflow in state[12]
            iv[13] += 1  # carry to state[13]
            # not to exceed 2^70 x 2^64 = 2^134 data size ??? <<<<

        # return None  # result in scramble_buf
    
    # '''
    # # as per definition - deprecated
    # def _quarterround(self, x, a, b, c, d):
    #     x[a] = (x[a] + x[b]) & 0xFFFFFFFF
    #     x[d] = self._rotate((x[d]^x[a]), 16)
    #     x[c] = (x[c] + x[d]) & 0xFFFFFFFF
    #     x[b] = self._rotate((x[b]^x[c]), 12)
    #
    #     x[a] = (x[a] + x[b]) & 0xFFFFFFFF
    #     x[d] = self._rotate((x[d]^x[a]), 8)
    #     x[c] = (x[c] + x[d]) & 0xFFFFFFFF
    #     x[b] = self._rotate((x[b]^x[c]), 7)
    #
    # def _rotate(self, v, n):        # aka ROTL32
    #     return ((v << n) & 0xFFFFFFFF) | (v >> (32 - n))
    # '''

    # surprisingly, the following tweaks/accelerations provide
    # about a 20-40% gain
    def _quarterround(self, x, a, b, c, d):
        #x=ptr32(xi) is already
        xa = x[a]
        xb = x[b]
        xc = x[c]
        xd = x[d]
        
        xa  = (xa + xb)  & 0xFFFFFFFF
        tmp =  xd ^ xa
        xd  = ((tmp << 16) & 0xFFFFFFFF) | (tmp >> 16)  # 16=32-16
        xc  = (xc + xd)  & 0xFFFFFFFF
        tmp =  xb ^ xc
        xb  = ((tmp << 12) & 0xFFFFFFFF) | (tmp >> 20)  # 20=32-12
        
        xa  = (xa + xb)  & 0xFFFFFFFF
        tmp =  xd ^ xa
        xd  = ((tmp <<  8) & 0xFFFFFFFF) | (tmp >> 24)  # 24=32-8
        xc  = (xc + xd)  & 0xFFFFFFFF
        tmp =  xb ^ xc
        xb  = ((tmp <<  7) & 0xFFFFFFFF) | (tmp >> 25)  # 25=32-7
        
        x[a] = xa
        x[b] = xb
        x[c] = xc
        x[d] = xd

    def _scramble_xor(self, data, length):
        stream=self.scramble_buf
        for i in range(length):
            if self.scramble_pos>=64: # used up
                self._chacha_scramble() # get some new bytes
            data[i] ^= stream[self.scramble_pos]
            self.scramble_pos+=1 # use up encryption buffer
