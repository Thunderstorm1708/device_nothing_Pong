/*
 * PenguinOS's libqcodec2_utils loads the QtiMapper extensions through
 * IQtiMapperExt_loadIMapperExt(), which only the newer mapper.qti exports, and
 * QC2Mapper5Helper refuses to import any buffer unless that load succeeded.
 * The extensions themselves (multi-view buffers for MV-HEVC) are only used for
 * auxiliary views, which cape cannot decode anyway.
 *
 * Hand out an extension object laid out like mapper.qti's (version 2 function
 * table) whose functions all report UNSUPPORTED.
 */
#include <cstdint>

struct native_handle;

namespace {

constexpr int kUnsupported = 7; // AIMapper_Error::UNSUPPORTED

int getMultiViewInfo(const native_handle*, uint32_t*) { return kUnsupported; }
int getBaseView(const native_handle*, uint32_t*) { return kUnsupported; }
int importViewBuffer(const native_handle*, uint32_t, const native_handle**) { return kUnsupported; }

struct ExtTable {
    uint32_t version;
    int (*getMultiViewInfo)(const native_handle*, uint32_t*);
    int (*getBaseView)(const native_handle*, uint32_t*);
    int (*importViewBuffer)(const native_handle*, uint32_t, const native_handle**);
};

struct Ext {
    void* reserved;
    void* impl;
    ExtTable* table;
};

ExtTable sTable = {2, getMultiViewInfo, getBaseView, importViewBuffer};
Ext sExt = {nullptr, &sTable, &sTable};

} // namespace

extern "C" int IQtiMapperExt_loadIMapperExt(void** ext) {
    if (!ext) return 3; // BAD_VALUE
    *ext = &sExt;
    return 0;
}
