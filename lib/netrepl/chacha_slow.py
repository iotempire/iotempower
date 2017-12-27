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

try:  # micropython
    from micropython import const
except:  # normal python
    def const(a):
        return a
import errno
import time


# -----------------------------------------------------------------------

def _add4(x, xi, s, si):
    # 4byte add x[xi] += s[si] little endian (lowest first)
    c = 0
    n = x[xi] + s[si] + c
    if n > 255:
        c = 1;n &= 255
    else:
        c = 0
    x[xi] = n;
    xi += 1;
    si += 1
    n = x[xi] + s[si] + c
    if n > 255:
        c = 1;n &= 255
    else:
        c = 0
    x[xi] = n;
    xi += 1;
    si += 1
    n = x[xi] + s[si] + c
    if n > 255:
        c = 1;n &= 255
    else:
        c = 0
    x[xi] = n;
    xi += 1;
    si += 1
    n = x[xi] + s[si] + c
    x[xi] = n & 255


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
    ROUNDS = const(12)  # ...10, 12, 20? [ulno] playing it safer with 12

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
        self._key_setup(key)
        self.iv_setup(iv)
        self.rounds = rounds
        self.tmp = [0, 0, 0, 0]

    def _key_setup(self, key):
        # """ key is converted to a list of 4-byte unsigned integers
        #     (32 bits).
        #
        #     Calling this routine with a key value effectively resets
        #     the context/instance.  Be sure to set the iv as well.
        # """
        if len(key) not in [16, 32]:
            raise Exception('key must be either 16 or 32 bytes')
        self.key_state = bytearray(64)
        key_state = self.key_state  # ref

        if len(key) == 16:
            key_state[0:16] = b"expand 16-byte k"
            key_state[16:32] = key[0:16]
            key_state[32:48] = key[0:16]
        elif len(key) == 32:
            key_state[0:16] = b"expand 32-byte k"
            key_state[16:48] = key[0:32]

        # 12*4 and 13*4 are reserved for the counter
        # 14*4 and 15*4 are reserved for the IV

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
        self.state = bytearray(self.key_state)
        self.scramble_buf = bytearray(self.key_state)
        self.scramble_pos = 64  # all used up to trigger new generation
        iv_state = self.state  # quick ref
        # iv_state[12] = 0 already 0
        # iv_state[13] = 0 already 0
        iv_state[56:64] = iv[0:8]

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
        if length is None:
            data_size = len(data)
        else:
            data_size = length
        self._scramble_xor(data, data_size)  # XOR the stream

    decrypt = encrypt  # same -> we are using xor against the evolving scramble-buffer

    def _chacha_scramble(self):  # 64 bytes in
        # """ self.state and other working structures are lists of
        #     4-byte unsigned integers (32 bits).
        #
        #     output must be converted to bytestring before return.
        # """

        s = self.state
        x = self.scramble_buf
        x[:] = s[:]

        for i in range(0, self.rounds, 2):
            # two rounds per iteration
            # self._quarterround(x, 0, 4, 8,12)
            # self._quarterround(x, 1, 5, 9,13)
            # self._quarterround(x, 2, 6,10,14)
            # self._quarterround(x, 3, 7,11,15)
            self._quarterround(x, 0, 16, 32, 48)
            self._quarterround(x, 4, 20, 36, 52)
            self._quarterround(x, 8, 24, 40, 56)
            self._quarterround(x, 12, 28, 44, 60)

            # self._quarterround(x, 0, 5,10,15)
            # self._quarterround(x, 1, 6,11,12)
            # self._quarterround(x, 2, 7, 8,13)
            # self._quarterround(x, 3, 4, 9,14)
            self._quarterround(x, 0, 20, 40, 60)
            self._quarterround(x, 4, 24, 44, 48)
            self._quarterround(x, 8, 28, 32, 52)
            self._quarterround(x, 12, 16, 36, 56)

        for i in range(0, 64, 4):
            # add 4 byte wise x[i] = (x[i] + s[i]) & 0xffffffff
            _add4(x, i, s, i)

        self.scramble_pos = 0  # reset pos

        # increment the iv.  In this case we increment words
        # 12*4 and 13*4 in little endian order.  This will work
        # nicely for data up to 2^70 bytes (1,099,511,627,776GB)
        # in length.  After that it is the user's responsibility
        # to generate a new nonce/iv.
        c = 1  # feed 1 to add in initial carry
        for j in range(48, 56):
            n = s[j] + c
            if n > 255:
                c = 1
                n &= 255
            else:
                c = 0
            s[j] = n
            # not to exceed 2^70 x 2^64 = 2^134 data size ??? <<<<

        # return None  # result in scramble_buf

    def _quarterround(self, x, a, b, c, d):
        # def _rotate(self, v, n):        # aka ROTL32
        #     return ((v << n) & 0xFFFFFFFF) | (v >> (32 - n))

        t = self.tmp
        # x[a] = (x[a] + x[b]) & 0xFFFFFFFF
        _add4(x, a, x, b)
        #     x[d] = self._rotate((x[d]^x[a]), 16)
        t[0] = x[d + 2] ^ x[a + 2]
        t[1] = x[d + 3] ^ x[a + 3]
        t[2] = x[d] ^ x[a]
        t[3] = x[d + 1] ^ x[a + 1]
        x[d] = t[0]
        x[d + 1] = t[1]
        x[d + 2] = t[2]
        x[d + 3] = t[3]

        # x[c] = (x[c] + x[d]) & 0xFFFFFFFF
        _add4(x, c, x, d)
        # x[b] = self._rotate((x[b]^x[c]), 12)
        t[0] = (x[b + 1] ^ x[c + 1]) << 4
        t[1] = (x[b + 2] ^ x[c + 2]) << 4
        t[2] = (x[b + 3] ^ x[c + 3]) << 4
        t[3] = (x[b] ^ x[c]) << 4
        x[b] = t[0] >> 8 + t[0] & 255
        x[b + 1] = t[1] >> 8 + t[1] & 255
        x[b + 2] = t[2] >> 8 + t[2] & 255
        x[b + 3] = t[3] >> 8 + t[3] & 255

        # x[a] = (x[a] + x[b]) & 0xFFFFFFFF
        _add4(x, a, x, b)
        # x[d] = self._rotate((x[d]^x[a]), 8)
        t[0] = x[d + 3] ^ x[a + 3]
        t[1] = x[d + 2] ^ x[a + 2]
        t[2] = x[d + 1] ^ x[a + 1]
        t[3] = x[d] ^ x[a]
        x[d] = t[0]
        x[d + 1] = t[1]
        x[d + 2] = t[2]
        x[d + 3] = t[3]

        # x[c] = (x[c] + x[d]) & 0xFFFFFFFF
        _add4(x, c, x, d)
        # x[b] = self._rotate((x[b]^x[c]), 7)
        t[0] = (x[b + 1] ^ x[c + 1]) << 7
        t[1] = (x[b + 2] ^ x[c + 2]) << 7
        t[2] = (x[b + 3] ^ x[c + 3]) << 7
        t[3] = (x[b] ^ x[c]) << 7
        x[b] = t[0] >> 15 + t[0] & 255
        x[b + 1] = t[1] >> 15 + t[1] & 255
        x[b + 2] = t[2] >> 15 + t[2] & 255
        x[b + 3] = t[3] >> 15 + t[3] & 255

    # # surprisingly, the following tweaks/accelerations provide
    # # about a 20-40% gain
    # def _quarterround(self, x, a, b, c, d):
    #     #x=A32(xi) is already
    #     xa = x[a]
    #     xb = x[b]
    #     xc = x[c]
    #     xd = x[d]
    #
    #     xa  = (xa + xb)  & 0xFFFFFFFF
    #     tmp =  xd ^ xa
    #     xd  = ((tmp << 16) & 0xFFFFFFFF) | (tmp >> 16)  # 16=32-16
    #     xc  = (xc + xd)  & 0xFFFFFFFF
    #     tmp =  xb ^ xc
    #     xb  = ((tmp << 12) & 0xFFFFFFFF) | (tmp >> 20)  # 20=32-12
    #
    #     xa  = (xa + xb)  & 0xFFFFFFFF
    #     tmp =  xd ^ xa
    #     xd  = ((tmp <<  8) & 0xFFFFFFFF) | (tmp >> 24)  # 24=32-8
    #     xc  = (xc + xd)  & 0xFFFFFFFF
    #     tmp =  xb ^ xc
    #     xb  = ((tmp <<  7) & 0xFFFFFFFF) | (tmp >> 25)  # 25=32-7
    #
    #     x[a] = xa
    #     x[b] = xb
    #     x[c] = xc
    #     x[d] = xd

    def _scramble_xor(self, data, length):
        stream = self.scramble_buf
        for i in range(length):
            if self.scramble_pos >= 64:  # used up
                self._chacha_scramble()  # get some new bytes
            data[i] ^= stream[self.scramble_pos]
            self.scramble_pos += 1  # use up encryption buffer
