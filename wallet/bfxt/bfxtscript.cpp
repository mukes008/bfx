#include "bfxtscript.h"

#include <bitset>
#include <boost/algorithm/hex.hpp>
#include <numeric>
#include <stdexcept>
#include <util.h>

#include "JsonStringQueue.h"
#include "base58.h"
#include "key.h"
#include "bfxtscript_burn.h"
#include "bfxtscript_issuance.h"
#include "bfxtscript_transfer.h"
#include "bfxtsendtokensonerecipientdata.h"
#include "transaction.h"

#include "script.h"

const std::string BFXTScript::IssuanceFlags::AggregationPolicy_Aggregatable_Str    = "aggregatable";
const std::string BFXTScript::IssuanceFlags::AggregationPolicy_NonAggregatable_Str = "nonaggregatable";

std::string BFXTScript::getHeader() const { return headerBin; }

std::string BFXTScript::getOpCodeBin() const { return opCodeBin; }

BFXTScript::TxType BFXTScript::getTxType() const { return txType; }

std::string BFXTScript::getParsedScriptHex() const { return parsedScriptHex; }

int BFXTScript::getProtocolVersion() const { return protocolVersion; }

void BFXTScript::setCommonParams(std::string Header, int ProtocolVersion, std::string OpCodeBin,
                                 std::string scriptHex)
{
    headerBin       = Header;
    protocolVersion = ProtocolVersion;
    opCodeBin       = OpCodeBin;
    txType          = CalculateTxType(opCodeBin);
    parsedScriptHex = scriptHex;
}

void BFXTScript::setEnableOpReturnSizeCheck(bool value) { enableOpReturnSizeCheck = value; }

bool BFXTScript::isOpReturnSizeCheckEnabled() const { return enableOpReturnSizeCheck; }

std::string BFXTScript::TransferInstructionToBinScript(const BFXTScript::TransferInstruction& inst)
{
    std::string result;

    std::string    skipBitStr = (inst.skipInput ? "1" : "0");
    std::bitset<5> outputIndexBits(inst.outputIndex);
    std::string    amountStr = NumberToHexBFXTAmount(inst.amount);

    std::string allStr = skipBitStr + "00" + outputIndexBits.to_string();
    if (allStr.size() != 8) {
        throw std::string("Unable to correctly construct transfer instruction with parameters: skip (" +
                          ToString(inst.skipInput) + "), output index (" + ToString(inst.outputIndex) +
                          ")");
    }
    uint8_t allUChar = static_cast<uint8_t>(std::bitset<8>(allStr).to_ulong());
    result += static_cast<char>(allUChar);

    result += boost::algorithm::unhex(amountStr);

    return result;
}

uint64_t BFXTScript::CalculateMetadataSize(const std::string& op_code_bin)
{
    if (op_code_bin.size() > 1) {
        throw std::runtime_error("Too large op_code");
    }
    const uint8_t char1 = static_cast<const uint8_t>(op_code_bin[0]);
    if (char1 == 0x01) {
        return 52;
    } else if (char1 == 0x02) {
        return 20;
    } else if (char1 == 0x03) {
        return 0;
    } else if (char1 == 0x04) {
        return 20;
    } else if (char1 == 0x05) {
        return 0;
    } else if (char1 == 0x06) {
        return 0;
    } else if (char1 == 0x10) {
        return 52;
    } else if (char1 == 0x11) {
        return 20;
    } else if (char1 == 0x12) {
        return 0;
    } else if (char1 == 0x13) {
        return 20;
    } else if (char1 == 0x14) {
        return 20;
    } else if (char1 == 0x15) {
        return 0;
    } else if (char1 == 0x20) {
        return 52;
    } else if (char1 == 0x21) {
        return 20;
    } else if (char1 == 0x22) {
        return 0;
    } else if (char1 == 0x23) {
        return 20;
    } else if (char1 == 0x24) {
        return 20;
    } else if (char1 == 0x25) {
        return 0;
    }
    throw std::runtime_error("Unknown OP_CODE (in hex): " + boost::algorithm::hex(op_code_bin));
}

BFXTScript::TxType BFXTScript::CalculateTxType(const std::string& op_code_bin)
{
    uint8_t char1 = static_cast<const uint8_t>(op_code_bin[0]);
    if (char1 <= 0x0F) {
        return TxType::TxType_Issuance;
    } else if (char1 <= 0x1F) {
        return TxType::TxType_Transfer;
    } else if (char1 <= 0x2F) {
        return TxType::TxType_Burn;
    } else {
        throw std::runtime_error("Unable to parse transaction type with unknown opcode (in hex): " +
                                 boost::algorithm::hex(op_code_bin));
    }
}

BFXTScript::TxType BFXTScript::CalculateTxTypeBFXTv3(const std::string& op_code_bin)
{
    uint8_t char1 = static_cast<const uint8_t>(op_code_bin[0]);
    if (char1 == 0x01) {
        return TxType::TxType_Issuance;
    } else if (char1 == 0x10) {
        return TxType::TxType_Transfer;
    } else if (char1 == 0x20) {
        return TxType::TxType_Burn;
    } else {
        throw std::runtime_error(
            "Unable to parse transaction type (BFXTv3) with unknown opcode (in hex): " +
            boost::algorithm::hex(op_code_bin));
    }
}

uint64_t BFXTScript::CalculateAmountSize(uint8_t firstChar)
{
    std::bitset<8> bits(firstChar);
    std::bitset<3> bits3(bits.to_string().substr(0, 3));
    unsigned long  s = bits3.to_ulong();
    if (s < 6) {
        return s + 1;
    } else {
        return 7;
    }
}

BFXTInt BFXTScript::ParseAmountFromLongEnoughString(const std::string& BinAmountStartsAtByte0,
                                                    int&               rawSize)
{
    if (BinAmountStartsAtByte0.size() < 2) {
        throw std::runtime_error("Too short a string to be parsed " +
                                 boost::algorithm::hex(BinAmountStartsAtByte0));
    }
    int amountSize = CalculateAmountSize(static_cast<uint8_t>(BinAmountStartsAtByte0[0]));
    if ((int)BinAmountStartsAtByte0.size() < amountSize) {
        throw std::runtime_error("Error parsing script: " + BinAmountStartsAtByte0 +
                                 "; the amount size is longer than what is available in the script");
    }
    rawSize = amountSize;
    return BFXTAmountHexToNumber(boost::algorithm::hex(BinAmountStartsAtByte0.substr(0, amountSize)));
}

std::string BFXTScript::ParseOpCodeFromLongEnoughString(const std::string& BinOpCodeStartsAtByte0)
{
    std::string result;
    for (unsigned i = 0; BinOpCodeStartsAtByte0.size(); i++) {
        const auto&   c  = BinOpCodeStartsAtByte0[i];
        const uint8_t uc = static_cast<const uint8_t>(c);
        result.push_back(c);
        // byte value 0xFF means that more OP_CODE bytes are required
        if (uc != 255) {
            break;
        } else {
            if (i + 1 == BinOpCodeStartsAtByte0.size()) {
                throw std::runtime_error("OpCode's last byte 0xFF indicates that there's more, but "
                                         "there's no more characters in the string.");
            }
        }
    }
    return result;
}

std::string BFXTScript::ParseMetadataFromLongEnoughString(const std::string& BinMetadataStartsAtByte0,
                                                          const std::string& op_code_bin,
                                                          const std::string& wholeScriptHex)
{
    int metadataSize = CalculateMetadataSize(op_code_bin);
    if ((int)BinMetadataStartsAtByte0.size() < metadataSize) {
        throw std::runtime_error("Error parsing script" +
                                 (wholeScriptHex.size() > 0 ? ": " + wholeScriptHex : "") +
                                 "; the metadata size is longer than what is available in the script");
    }
    return BinMetadataStartsAtByte0.substr(0, metadataSize);
}

std::string
BFXTScript::ParseBFXTv3MetadataFromLongEnoughString(const std::string& BinMetadataSizeStartsAtByte0,
                                                    const std::string& wholeScriptHex)
{
    std::string ScriptBin = BinMetadataSizeStartsAtByte0;

    if (ScriptBin.size() == 0) {
        return "";
    }
    if (ScriptBin.size() < 4) {
        throw std::runtime_error(
            "The data remaining cannot fit metadata start flag, which is 4 bytes: " +
            boost::algorithm::hex(ScriptBin) + ", starting from " + boost::algorithm::hex(ScriptBin));
    }

    std::string metadataSizeStr = ScriptBin.substr(0, 4);
    ScriptBin.erase(ScriptBin.begin(), ScriptBin.begin() + metadataSizeStr.size());

    uint32_t metadataSize;
    memcpy(&metadataSize, &metadataSizeStr.front(), 4);

    FromBigEndianToThisEndianness(metadataSize);

    if (ScriptBin.size() != metadataSize) {
        throw std::runtime_error(
            "The size of the metadata found is not equal to the available size of the data left: " +
            wholeScriptHex);
    }

    return ScriptBin;
}

std::string
BFXTScript::ParseTokenSymbolFromLongEnoughString(const std::string& BinTokenSymbolStartsAtByte0)
{
    if ((int)BinTokenSymbolStartsAtByte0.size() < 0) {
        throw std::runtime_error(
            "Error parsing script (starting at this point a symbol is expected). " +
            (BinTokenSymbolStartsAtByte0.size() > 0 ? ": " + BinTokenSymbolStartsAtByte0 : "") +
            "; the token symbol size is longer than what is available in the script");
    }
    const std::string& ScriptBin = BinTokenSymbolStartsAtByte0;
    std::string        result;
    result = ScriptBin.substr(0, 5);
    // drop 0x01 chars from the beginning
    result.erase(std::remove_if(result.begin(), result.end(),
                                [](char c) { return static_cast<uint8_t>(c) == 0x20; }),
                 result.end());

    auto it =
        std::find_if(result.begin(), result.end(), [](char c) { return !IsTokenSymbolCharValid(c); });
    if (it != result.end()) {
        throw std::runtime_error("Invalid token symbol. Token symbols can only contain English letters "
                                 "and numbers. The current name \"" +
                                 result + "\", has an invalid character: \"" + std::string(1, *it) +
                                 "\"");
    }

    if (result.size() == 0) {
        throw std::runtime_error("Invalid token symbol; it cannot be empty.");
    }
    return result;
}

std::vector<BFXTScript::TransferInstruction> BFXTScript::ParseTransferInstructionsFromLongEnoughString(
    const std::string& BinInstructionsStartFromByte0, int& totalRawSize)
{
    std::string                      toParse = BinInstructionsStartFromByte0;
    std::vector<TransferInstruction> result;
    totalRawSize = 0;
    for (int i = 0;; i++) {
        if (toParse.size() <= 1) {
            break;
        }

        // one byte of flags, and then N bytes for the amount
        TransferInstruction transferInst;
        transferInst.firstRawByte = static_cast<unsigned char>(toParse[0]);
        transferInst.rawAmount    = toParse.substr(1, CalculateAmountSize(toParse[1]));
        int currentSize           = 1 + transferInst.rawAmount.size();
        totalRawSize += currentSize;
        toParse.erase(toParse.begin(), toParse.begin() + currentSize);

        // parse data from raw
        std::bitset<8> rawByte(transferInst.firstRawByte);
        std::bitset<5> outputIndex(rawByte.to_string().substr(3, 5));
        transferInst.skipInput   = rawByte.test(7); // first big-endian bit (is the last one in bitset)
        transferInst.outputIndex = static_cast<int>(outputIndex.to_ulong());

        transferInst.amount = BFXTAmountHexToNumber(boost::algorithm::hex(transferInst.rawAmount));

        // push to the vector
        result.push_back(transferInst);
    }
    return result;
}

std::vector<BFXTScript::TransferInstruction>
BFXTScript::ParseBFXTv3TransferInstructionsFromLongEnoughString(
    const std::string& BinInstructionsStartFromByte0, int& totalRawSize)
{
    std::string                      toParse = BinInstructionsStartFromByte0;
    std::vector<TransferInstruction> result;
    totalRawSize = 0;

    if (toParse.size() < 1) {
        throw std::runtime_error("Transfer instructions do not contain the number of transfer "
                                 "instructions in their first byte");
    }

    int numOfTIs = static_cast<unsigned char>(toParse[0]);
    toParse.erase(toParse.begin(), toParse.begin() + 1);
    totalRawSize += 1;

    if (numOfTIs <= 0) {
        throw std::runtime_error("The number of transfer instructions cannot be zero.");
    }

    for (int i = 0; i < numOfTIs; i++) {
        if (toParse.size() <= 1) {
            throw std::runtime_error("Transfer instruction number " + ToString(i) + " has a size <= 1");
        }

        // one byte of flags, and then N bytes for the amount
        TransferInstruction transferInst;
        transferInst.firstRawByte = static_cast<unsigned char>(toParse[0]);
        transferInst.rawAmount    = toParse.substr(1, CalculateAmountSize(toParse[1]));
        int currentSize           = 1 + transferInst.rawAmount.size();
        totalRawSize += currentSize;
        toParse.erase(toParse.begin(), toParse.begin() + currentSize);

        // parse data from raw
        std::bitset<8> rawByte(transferInst.firstRawByte);
        std::bitset<5> outputIndex(rawByte.to_string().substr(3, 5));
        transferInst.skipInput   = rawByte.test(7); // first big-endian bit (is the last one in bitset)
        transferInst.outputIndex = static_cast<int>(outputIndex.to_ulong());

        transferInst.amount = BFXTAmountHexToNumber(boost::algorithm::hex(transferInst.rawAmount));

        // push to the vector
        result.push_back(transferInst);
    }
    return result;
}

std::shared_ptr<BFXTScript> BFXTScript::ParseScript(const std::string& scriptHex)
{
    try {
        std::string scriptBin = boost::algorithm::unhex(scriptHex);

        if (scriptBin.size() < 3) {
            throw std::runtime_error("Too short script");
        }
        std::string              header        = scriptBin.substr(0, 3);
        static const std::string BFXTHexPrefix = "4e54";
        if (header.substr(0, 2) != boost::algorithm::unhex(BFXTHexPrefix)) {
            throw std::runtime_error("BFXT script prefix is invalid for " + scriptHex);
        }
        int protocolVersion = static_cast<decltype(protocolVersion)>(static_cast<uint8_t>(header[2]));

        // drop header bytes
        scriptBin.erase(scriptBin.begin(), scriptBin.begin() + 3);

        std::string opCodeBin = ParseOpCodeFromLongEnoughString(scriptBin);
        TxType      txType;
        if (protocolVersion == 1) {
            txType = CalculateTxType(opCodeBin);
        } else if (protocolVersion == 3) {
            txType = CalculateTxTypeBFXTv3(opCodeBin);
        } else {
            throw std::runtime_error("Unknown protocol version " + ToString(protocolVersion) +
                                     " in script: " + scriptHex);
        }
        // drop the OP_CODE parsed part
        scriptBin.erase(scriptBin.begin(), scriptBin.begin() + opCodeBin.size());

        std::shared_ptr<BFXTScript> result_;

        if (txType == TxType::TxType_Issuance) {
            if (protocolVersion == 1) {
                result_ = BFXTScript_Issuance::ParseIssuancePostHeaderData(scriptBin, opCodeBin);
            } else if (protocolVersion == 3) {
                result_ = BFXTScript_Issuance::ParseBFXTv3IssuancePostHeaderData(scriptBin);
            } else {
                throw std::runtime_error("Unknown protocol version " + ToString(protocolVersion) +
                                         " in script: " + scriptHex);
            }
        } else if (txType == TxType::TxType_Transfer) {
            if (protocolVersion == 1) {
                result_ = BFXTScript_Transfer::ParseTransferPostHeaderData(scriptBin, opCodeBin);
            } else if (protocolVersion == 3) {
                result_ = BFXTScript_Transfer::ParseBFXTv3TransferPostHeaderData(scriptBin);
            } else {
                throw std::runtime_error("Unknown protocol version " + ToString(protocolVersion) +
                                         " in script: " + scriptHex);
            }
        } else if (txType == TxType::TxType_Burn) {
            if (protocolVersion == 1) {
                result_ = BFXTScript_Burn::ParseBurnPostHeaderData(scriptBin, opCodeBin);
            } else if (protocolVersion == 3) {
                result_ = BFXTScript_Burn::ParseBFXTv3BurnPostHeaderData(scriptBin);
            } else {
                throw std::runtime_error("Unknown protocol version " + ToString(protocolVersion) +
                                         " in script: " + scriptHex);
            }
        } else {
            throw std::runtime_error("Unknown transaction type to parse in script: " + scriptHex);
        }
        result_->setCommonParams(header, protocolVersion, opCodeBin, scriptHex);

        return result_;

    } catch (std::exception& ex) {
        throw std::runtime_error("Unable to parse hex script: " + scriptHex + "; reason: " + ex.what());
    }
}

BFXTScript::IssuanceFlags BFXTScript::IssuanceFlags::ParseIssuanceFlag(uint8_t flags)
{
    IssuanceFlags  result;
    std::bitset<8> bits(flags);
    // first 3 bits
    result.divisibility = static_cast<decltype(result.divisibility)>(
        std::bitset<3>(bits.to_string().substr(0, 3)).to_ulong());
    result.locked = bits.test(4); // 4th bit (3rd bit from the lsb, 7-(4-1)=3)
    // 5th + 6th bits
    int aggrPolicy = static_cast<decltype(result.divisibility)>(
        std::bitset<2>(bits.to_string().substr(4, 2)).to_ulong());
    switch (aggrPolicy) {
    case 0: // 00
        result.aggregationPolicy = AggregationPolicy::AggregationPolicy_Aggregatable;
        break;
    case 2: // 10
        result.aggregationPolicy = AggregationPolicy::AggregationPolicy_NonAggregatable;
        break;
    default: // everything else is not known (01 and 11)
        throw std::runtime_error("Unknown aggregation policy: " + std::to_string(aggrPolicy));
    }
    return result;
}

std::string BFXTScript::NumberToHexBFXTAmount(const BFXTInt& num, bool caps)
{
    std::string numStr     = ToString(num);
    int         zerosCount = 0;
    // numbers less than 32 can fit in a single byte with no exponent
    if (num >= 32 && ToString(BFXTScript::GetTrailingZeros(num)).size() <= 12) {
        for (unsigned i = 0; i < numStr.size(); i++) {
            if (numStr[numStr.size() - i - 1] == '0') {
                zerosCount++;
            } else {
                break;
            }
        }
    }

    BFXTInt mantissaDecimal = FromString<BFXTInt>(numStr.substr(0, numStr.size() - zerosCount));
    // create binary values of mantissa and exponent (exponent's zero-count)
    std::string mantissaStr = ConvertToBitString(mantissaDecimal);
    std::string exponentStr = ConvertToBitString(zerosCount);
    {
        // trim mantissa leading zeros
        int toTrim = 0;
        for (unsigned i = 0; i < mantissaStr.size(); i++) {
            if (mantissaStr[i] == '0') {
                toTrim++;
            } else {
                break;
            }
        }
        mantissaStr = mantissaStr.substr(toTrim, mantissaStr.size() - toTrim);
    }
    {
        // trim exponent leading zeros
        int toTrim = 0;
        for (unsigned i = 0; i < exponentStr.size(); i++) {
            if (exponentStr[i] == '0') {
                toTrim++;
            } else {
                break;
            }
        }
        exponentStr = exponentStr.substr(toTrim, exponentStr.size() - toTrim);
    }

    int         mantissaSize = 0;
    int         exponentSize = 0;
    std::string header;

    if (mantissaStr.size() <= 5 && exponentStr.size() == 0) {
        header       = "000";
        mantissaSize = 5;
        exponentSize = 0;
    } else if (mantissaStr.size() <= 9 && exponentStr.size() <= 4) {
        header       = "001";
        mantissaSize = 9;
        exponentSize = 4;
    } else if (mantissaStr.size() <= 17 && exponentStr.size() <= 4) {
        header       = "010";
        mantissaSize = 17;
        exponentSize = 4;
    } else if (mantissaStr.size() <= 25 && exponentStr.size() <= 4) {
        header       = "011";
        mantissaSize = 25;
        exponentSize = 4;
    } else if (mantissaStr.size() <= 34 && exponentStr.size() <= 3) {
        header       = "100";
        mantissaSize = 34;
        exponentSize = 3;
    } else if (mantissaStr.size() <= 42 && exponentStr.size() <= 3) {
        header       = "101";
        mantissaSize = 42;
        exponentSize = 3;
    } else if (mantissaStr.size() <= 54 && exponentStr.size() == 0) {
        header       = "11";
        mantissaSize = 54;
        exponentSize = 0;
    } else {
        throw std::runtime_error("Unable to encode the number " + ToString(num) +
                                 " to BFXT amount hex; its mantissa and exponent do not fit in the "
                                 "expected binary representation.");
    }

    mantissaStr = std::string(mantissaSize - mantissaStr.size(), '0') + mantissaStr;
    exponentStr = std::string(exponentSize - exponentStr.size(), '0') + exponentStr;

    std::string finalBinString = header + mantissaStr + exponentStr;

    if ((finalBinString.size() % 8) != 0) {
        throw std::runtime_error("The constructed binary string does not have the expected size.");
    }

    std::string encodedData;
    encodedData.resize(finalBinString.size() / 8);
    for (unsigned i = 0; i < finalBinString.size(); i += 8) {
        boost::dynamic_bitset<> singleByteBitset(finalBinString.substr(i, 8));
        encodedData[i / 8] = static_cast<char>(singleByteBitset.to_ulong());
    }
    if (caps) {
        std::string res = boost::algorithm::hex(encodedData);
        std::transform(res.begin(), res.end(), res.begin(), ::toupper);
        return res;
    } else {
        std::string res = boost::algorithm::hex(encodedData);
        std::transform(res.begin(), res.end(), res.begin(), ::tolower);
        return res;
    }
}

std::string BFXTScript::GetMetadataAsString(const BFXTScript*   bfxtscript,
                                            const CTransaction& tx) noexcept
{
    if (!bfxtscript) {
        return "";
    }

    std::string textMetadata;
    // decompress, or return uncompress hex data in an error
    try {
        textMetadata = bfxtscript->getInflatedMetadata();
        json_spirit::Value encNode;
        json_spirit::read_or_throw(textMetadata, encNode);
        json_spirit::Value ser = json_spirit::find_value(encNode.get_obj(), METADATA_SER_FIELD__VERSION);
        if (!(ser == json_spirit::Value::null)) {
            boost::optional<std::string> decError;
            std::string dec = CTransaction::DecryptMetadataOfTx(textMetadata, tx.GetHash(), decError);
            if (decError) {
                printf("Message decryption failed: %s\n", decError->c_str());
            } else {
                textMetadata = std::move(dec);
            }
        }
    } catch (std::exception& ex) {
        printf("Failed at decompressing \"%s\". Reason: %s\n", textMetadata.c_str(), ex.what());
    } catch (...) {
        printf("Failed at decompressing \"%s\". Unknown exception.\n", textMetadata.c_str());
    }

    return textMetadata;
}

json_spirit::Value BFXTScript::GetMetadataAsJson(const BFXTScript*   bfxtscript,
                                                 const CTransaction& tx) noexcept
{
    if (!bfxtscript) {
        return json_spirit::Value();
    }
    std::string textMetadata = GetMetadataAsString(bfxtscript, tx);

    // if empty, return null
    if (textMetadata.empty()) {
        return json_spirit::Value();
    }

    // try to parse json data, return
    try {
        json_spirit::Value res;
        json_spirit::read_or_throw(textMetadata, res);
        return res;
    } catch (...) // json_spirit throws Error_position too
    {
        json_spirit::Object root;
        root.push_back(json_spirit::Pair("error", "json_parsing_failed"));
        root.push_back(json_spirit::Pair("json_string", textMetadata));
        return json_spirit::Value(root);
    }
}

bool BFXTScript::IsBFXTTokenSymbolValid(const std::string& symbol)
{
    if (symbol.size() > 5) {
        return false;
    }
    auto it =
        std::find_if(symbol.begin(), symbol.end(), [](char c) { return !IsTokenSymbolCharValid(c); });
    return it == symbol.end();
}

bool BFXTScript::IsTokenSymbolCharValid(const char c) { return isalnum(c); }

BFXTInt BFXTScript::GetTrailingZeros(const BFXTInt& num)
{
    if (num == 0) {
        return 0;
    }
    std::string numStr   = ToString(num);
    int         toRemove = 0;
    for (int i = numStr.size() - 1; i >= 0; i--) {
        if (numStr[i] == '0') {
            toRemove++;
        } else {
            break;
        }
    }
    return FromString<BFXTInt>(numStr.substr(0, numStr.size() - toRemove));
}

BFXTInt BFXTScript::BFXTAmountHexToNumber(std::string hexVal)
{
#ifdef __BYTE_ORDER__
    static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__,
                  "Non little-endian systems are not supported");
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
    throw std::runtime_error("Big and pdp endian systems are not supported");
#endif
#endif

    // remove spaces
    hexVal.erase(std::remove_if(hexVal.begin(), hexVal.end(), [](char c) { return c == ' '; }),
                 hexVal.end());

    if (!boost::regex_match(hexVal, HexBytexRegex)) {
        throw std::runtime_error("invalid hex binary string");
    }
    std::string bin = boost::algorithm::unhex(hexVal);
    if (bin.size() > 7) {
        throw std::out_of_range("Amount can't be bigger than 7 bytes.");
    }

    // convert the hex to a bitset
    boost::dynamic_bitset<> bits(bin.size() * 8, 0);
    for (unsigned i = 0; i < bin.size(); i++) {
        set_in_range(bits, bin[bin.size() - i - 1], i * 8 + 0, (i + 1) * 8);
    }

    bool bit0 = bits[bits.size() - 1];
    bool bit1 = bits[bits.size() - 2];
    bool bit2 = bits[bits.size() - 3];

    // sizes in bits
    int headerSize   = 0;
    int mantissaSize = 0;
    int exponentSize = 0;
    if (bit0 && bit1) {
        headerSize   = 2;
        mantissaSize = 54;
        exponentSize = 0;
    } else {
        headerSize = 3;
        if (bit0 == 0 && bit1 == 0 && bit2 == 0) {
            mantissaSize = 5;
            exponentSize = 0;
        } else if (bit0 == 0 && bit1 == 0 && bit2 == 1) {
            mantissaSize = 9;
            exponentSize = 4;
        } else if (bit0 == 0 && bit1 == 1 && bit2 == 0) {
            mantissaSize = 17;
            exponentSize = 4;
        } else if (bit0 == 0 && bit1 == 1 && bit2 == 1) {
            mantissaSize = 25;
            exponentSize = 4;
        } else if (bit0 == 1 && bit1 == 0 && bit2 == 0) {
            mantissaSize = 34;
            exponentSize = 3;
        } else if (bit0 == 1 && bit1 == 0 && bit2 == 1) {
            mantissaSize = 42;
            exponentSize = 3;
        } else {
            throw std::logic_error("Unexpected binary structure. This should never happen.");
        }
    }

    // ensure that the total size makes sense
    {
        unsigned totalBitSize = headerSize + mantissaSize + exponentSize;
        if ((totalBitSize / 8) != bin.size() || (totalBitSize % 8) != 0) {
            throw std::logic_error("The total bits don't make a byte. This should never happen.");
        }
    }

    std::string bitString = boost::to_string(bits);
    std::string mantissa  = bitString.substr(headerSize, mantissaSize);
    std::string exponent  = bitString.substr(headerSize + mantissaSize, exponentSize);

    constexpr unsigned int digits =
        (std::numeric_limits<BFXTInt>::digits <= 512 ? std::numeric_limits<BFXTInt>::digits : 512);

    static_assert(digits <= 512, "Very large type for BFXTInt");
    static_assert(digits >= 64, "Very short type for BFXTInt");

    return static_cast<BFXTInt>(std::bitset<digits>(mantissa).to_ullong()) *
           static_cast<BFXTInt>(
               boost::multiprecision::pow(BFXTInt(10), boost::dynamic_bitset<>(exponent).to_ulong()));
}

std::string
BFXTScript::EncryptMetadataWithEphemeralKey(const StringViewT data, const CKey& publicKey,
                                            Crypto_HighLevel::EncryptionAlgorithm     encAlgo,
                                            Crypto_HighLevel::AuthKeyRatchetAlgorithm ratchetAlgo,
                                            Crypto_HighLevel::AuthenticationAlgorithm authAlgo)
{
    CKey ephemeralKey;
    ephemeralKey.MakeNewKey(true);

    return EncryptMetadata(data, ephemeralKey, publicKey, encAlgo, ratchetAlgo, authAlgo);
}

std::string BFXTScript::EncryptMetadata(const StringViewT data, const CKey& privateKey,
                                        const CKey& publicKey, CHL::EncryptionAlgorithm encAlgo,
                                        CHL::AuthKeyRatchetAlgorithm ratchetAlgo,
                                        CHL::AuthenticationAlgorithm authAlgo)
{

    std::array<uint8_t, 32> sharedKey =
        CKey::GenerateSharedSecretFromPrivateAndPublicKey(privateKey, publicKey);

    auto       cipherData           = CHL::EncryptMessage(CHL::Bytes(data.cbegin(), data.cend()),
                                          CHL::SecureBytes(sharedKey.cbegin(), sharedKey.cend()),
                                          encAlgo, ratchetAlgo, authAlgo);
    CHL::Bytes serializedCipherData = CHL::EncryptMessageOutput::Serialize(cipherData);

    if (static_cast<int64_t>(serializedCipherData.size()) >= static_cast<int64_t>(INT_MAX)) {
        throw std::runtime_error("Data is too big to be serialized");
    }

    json_spirit::Object root;
    using JPair                 = json_spirit::Pair;
    namespace BA                = boost::algorithm;
    auto        sourcePubKey    = privateKey.GetPubKey().Raw();
    std::string sourcePubKeyHex = BA::hex(std::string(sourcePubKey.cbegin(), sourcePubKey.cend()));
    auto        targetPubKey    = publicKey.GetPubKey().Raw();
    std::string targetPubKeyHex = BA::hex(std::string(targetPubKey.cbegin(), targetPubKey.cend()));
    root.push_back(JPair(METADATA_SER_FIELD__VERSION, 2));
    root.push_back(JPair(METADATA_SER_FIELD__SOURCE_PUBLIC_KEY_HEX, sourcePubKeyHex));
    root.push_back(JPair(METADATA_SER_FIELD__TARGET_PUBLIC_KEY_HEX, targetPubKeyHex));
    root.push_back(
        JPair(METADATA_SER_FIELD__CIPHER_BASE64, EncodeBase64(CHL::ToString(serializedCipherData))));

    // write the header then the data to the result
    std::string result = json_spirit::write(json_spirit::Value(root));
    boost::trim(result);
    return result;
}

std::string GetStrValue(const json_spirit::Object& obj, const std::string& name)
{
    auto v = json_spirit::find_value(obj, name);
    if (v == json_spirit::Value::null) {
        throw std::runtime_error("Could not find json object: " + name);
    }
    if (v.type() != json_spirit::Value_type::str_type) {
        throw std::runtime_error("For object " + name +
                                 " the expected type is string, but found type with index " +
                                 std::to_string(v.type()));
    }
    return v.get_str();
}

int GetIntValue(const json_spirit::Object& obj, const std::string& name)
{
    auto v = json_spirit::find_value(obj, name);
    if (v == json_spirit::Value::null) {
        throw std::runtime_error("Could not find json object: " + name);
    }
    if (v.type() != json_spirit::Value_type::int_type) {
        throw std::runtime_error("For object " + name +
                                 " the expected type is int, but found type with index " +
                                 std::to_string(v.type()));
    }
    return v.get_int();
}

std::string BFXTScript::DecryptMetadata(const StringViewT data, const CKey& privateKey)
{
    if (data.empty()) {
        throw std::runtime_error("Requested decryption of empty metadata data");
    }
    if (data[0] != '{') {
        throw std::runtime_error("Unexpected header while attempting to decrypt metadata");
    }

    json_spirit::Value jsonHeader;
    try {
        json_spirit::read_or_throw(data.to_string(), jsonHeader);
    } catch (std::exception& ex) {
        throw std::runtime_error("Unable to parse json header of encrypted metadata: Error: " +
                                 std::string(ex.what()));
    } catch (...) {
        throw std::runtime_error("Unable to parse json header of encrypted metadata: Unknown error");
    }

    if (jsonHeader.type() != json_spirit::Value_type::obj_type) {
        throw std::runtime_error("Invalid encrypted metadata header type");
    }

    json_spirit::Object jsonHeaderObj = jsonHeader.get_obj();
    int                 version       = GetIntValue(jsonHeaderObj, METADATA_SER_FIELD__VERSION);
    std::string sourcePubKeyHex = GetStrValue(jsonHeaderObj, METADATA_SER_FIELD__SOURCE_PUBLIC_KEY_HEX);
    std::string targetPubKeyHex = GetStrValue(jsonHeaderObj, METADATA_SER_FIELD__TARGET_PUBLIC_KEY_HEX);
    std::string cipherBase64    = GetStrValue(jsonHeaderObj, METADATA_SER_FIELD__CIPHER_BASE64);

    if (version != 2) {
        throw std::runtime_error("Unknown serialization version of encrypted metadata");
    }

    std::vector<uint8_t> pubKeyToUse;
    {
        std::vector<uint8_t> sourcePubKeyRaw;
        boost::algorithm::unhex(sourcePubKeyHex.cbegin(), sourcePubKeyHex.cend(),
                                std::back_inserter(sourcePubKeyRaw));
        std::vector<uint8_t> targetPubKeyRaw;
        boost::algorithm::unhex(targetPubKeyHex.cbegin(), targetPubKeyHex.cend(),
                                std::back_inserter(targetPubKeyRaw));

        if (privateKey.GetPubKey().Raw() == sourcePubKeyRaw) {
            pubKeyToUse = targetPubKeyRaw;
        }
        if (privateKey.GetPubKey().Raw() == targetPubKeyRaw) {
            pubKeyToUse = sourcePubKeyRaw;
        }
        if (pubKeyToUse.empty()) {
            throw std::runtime_error(
                "Invalid private key. The private key failed to produce any of the public key.");
        }
    }

    if (!IsCanonicalPubKey(pubKeyToUse)) {
        std::runtime_error("Public key provided is not a canonical public key");
    }

    CKey pubKey;
    if (!pubKey.SetPubKey(pubKeyToUse)) {
        throw std::runtime_error(
            "Failed to set public key in a ckey object. Possibly an invalid public key.");
    }

    std::array<uint8_t, 32> sharedSecret = privateKey.GenerateSharedSecretFromThisPrivateKey(pubKey);

    std::string cipher = DecodeBase64(cipherBase64);

    CHL::EncryptMessageOutput encryptedMessage =
        CHL::EncryptMessageOutput::Deserialize(CHL::ToBytes(cipher));

    CHL::Bytes result = CHL::DecryptMessage(
        encryptedMessage, CHL::SecureBytes(sharedSecret.cbegin(), sharedSecret.cend()));

    return std::string(std::make_move_iterator(result.begin()), std::make_move_iterator(result.end()));
}

std::string BFXTScript::EncryptMetadataBeforeSend(const StringViewT bfxtmetadata,
                                                  const CKey&       inputPrivateKey,
                                                  const StringViewT recipientAddress)
{
    std::string          result;
    CTxDB                txdb;
    std::vector<uint8_t> pubKeyRaw;
    std::string          addr(recipientAddress.cbegin(), recipientAddress.cend());
    bool                 pubkeyFound = txdb.ReadAddressPubKey(CBitcoinAddress(addr), pubKeyRaw);
    if (!pubkeyFound) {
        throw std::runtime_error("The public key for the address " + addr +
                                 " was not found. This means that the owner of this "
                                 "address never made a transaction with it. Please "
                                 "choose an address, which was used before.");
    }
    CKey pubKeyCKey;
    if (!pubKeyCKey.SetPubKey(pubKeyRaw)) {
        throw std::runtime_error("The public key for the address " + addr +
                                 " was found, but it seems to be invalid. Please try another address. "
                                 "Also please consider reporting this to the BFX Team.");
    }
    CBitcoinAddress addrRecalc(pubKeyCKey.GetPubKey().GetID());
    if (addr != addrRecalc.ToString()) {
        throw std::runtime_error("The public key for the address " + addr +
                                 " was found, but failed to reproduce the same address. This indicates "
                                 "that the public key is invalid. Please try another address, and "
                                 "consider reporting this to the BFX Team.");
    }
    try {
        result = BFXTScript::EncryptMetadata(
            bfxtmetadata, inputPrivateKey, pubKeyCKey, CHL::EncryptionAlgorithm::Enc_XSalsa20Poly1305,
            CHL::AuthKeyRatchetAlgorithm::Ratchet_Sha256, CHL::AuthenticationAlgorithm::Auth_Poly1305);
    } catch (std::exception& ex) {
        throw std::runtime_error("Failed to encrypt metadata. Error: " + std::string(ex.what()));
    }
    return result;
}

std::string RawBFXTMetadataBeforeSend::applyMetadataEncryption(
    const CTransaction& wtxNew, const std::vector<BFXTSendTokensOneRecipientData>& recipients) const
{
    if (this->encrypt && !this->metadata.empty()) {
        // get public key
        std::vector<CKey> keys;
        for (const CTxIn& in : wtxNew.vin) {
            std::vector<CKey> tempKeys =
                CTransaction::GetThisWalletKeysOfTx(in.prevout.hash, in.prevout.n);
            keys.insert(keys.end(), std::make_move_iterator(tempKeys.begin()),
                        std::make_move_iterator(tempKeys.end()));
        }
        if (keys.empty()) {
            throw std::runtime_error("An unexpected error happened. A key that was used "
                                     "in the input was not found in the wallet. Please "
                                     "report this to bfx develoers.");
        }
        CKey inputPrivateKey = keys.front();

        if (recipients.empty()) {
            throw std::runtime_error("Empty BFXT recipient list");
        }

        // put recipient addresses in a set to ensure there is only one
        std::set<std::string> recipientsSet;
        std::transform(recipients.cbegin(), recipients.cend(),
                       std::inserter(recipientsSet, recipientsSet.begin()),
                       [](const BFXTSendTokensOneRecipientData& r) { return r.destination; });
        if (recipientsSet.size() != 1) {
            throw std::runtime_error("Cannot send encrypted BFXT metadata to multiple recipients.");
        }
        std::string recipient = *recipientsSet.begin();
        return BFXTScript::EncryptMetadataBeforeSend(this->metadata, inputPrivateKey, recipient);
    } else {
        return this->metadata;
    }
}
