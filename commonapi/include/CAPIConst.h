#ifndef _CAPI_CONST_H_
#define _CAPI_CONST_H_

#include <string>
#include <list>

const std::string RunModel_All = "All";
const std::string RunModel_Method = "Method";
const std::string RunModel_Broadcast = "Broadcast";
const std::string RunModel_Notifier = "Notifier";
const std::string RunModel_Getter = "Getter";
const std::string RunModel_Setter = "Setter";

extern std::string CurrentModel;

#endif