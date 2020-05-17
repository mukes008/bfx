#include "udaddress.h"

#include "bfxt/bfxttools.h"

const std::string  UnstoppableDomainsRegexStr = R"(^[a-zA-Z0-9\-]+\.(?:crypto|zil)$)";
const boost::regex UnstoppableDomainsRegex(UnstoppableDomainsRegexStr);
const std::string  UnstoppableDomainsV1URLBase = "https://unstoppabledomains.com/api/v1/";

bool IsUDAddressSyntaxValid(const StringViewT UDDomain)
{
    return boost::regex_match(UDDomain.begin(), UDDomain.end(), UnstoppableDomainsRegex);
}

boost::optional<std::string> GetUDAddressAPICall(const StringViewT UDDomain)
{
    boost::smatch unstopDomainsArgMatch;
    if (IsUDAddressSyntaxValid(UDDomain)) {
        return UnstoppableDomainsV1URLBase + UDDomain.to_string();
    } else {
        return boost::none;
    }
}

boost::optional<std::string> GetBFXAddressFromUDAddress(const StringViewT UDDomain)
{
    std::string UDDomainR;
    // convert the address to lower-case to avoid issues with unstoppable domain
    std::transform(UDDomain.cbegin(), UDDomain.cend(), std::back_inserter(UDDomainR), ::tolower);
    try {
        boost::optional<std::string> APICall = GetUDAddressAPICall(UDDomain);
        if (!APICall) {
            return boost::none;
        }
        std::string        udData = cURLTools::GetFileFromHTTPS(*APICall, 30, false, true);
        json_spirit::Value val;
        json_spirit::read_or_throw(udData, val);
        json_spirit::Object addressesObj = BFXTTools::GetObjectField(val.get_obj(), "addresses");
        return BFXTTools::GetStrField(addressesObj, "BFX");
    } catch (const std::exception& ex) {
        printf("Failed to get address from unstoppable domain: %s", ex.what());
        return boost::none;
    }
}
