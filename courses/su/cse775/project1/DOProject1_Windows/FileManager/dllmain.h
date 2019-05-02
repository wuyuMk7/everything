// dllmain.h : Declaration of module class.

class CFileManagerModule : public ATL::CAtlDllModuleT< CFileManagerModule >
{
public :
	DECLARE_LIBID(LIBID_FileManagerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FILEMANAGER, "{1A08DE44-509E-4648-A876-D9612A04C599}")
};

extern class CFileManagerModule _AtlModule;
