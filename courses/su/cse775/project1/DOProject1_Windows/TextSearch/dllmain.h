// dllmain.h : Declaration of module class.

class CTextSearchModule : public ATL::CAtlDllModuleT< CTextSearchModule >
{
public :
	DECLARE_LIBID(LIBID_TextSearchLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TEXTSEARCH, "{0294513C-B5BA-4FB3-98FC-99A52A55AFE4}")
};

extern class CTextSearchModule _AtlModule;
