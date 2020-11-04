#include "pch.h"

const char* mdlformatstrings[] = {
	"basic",
	"chunk"
};

const char* mdlformatfiletypes[] = {
	".sa1mdl",
	".sa2mdl"
};

ModelInfo* LoadModel(ModelInfo** info, const char* name, ModelFormat format) {
	PrintDebug("[Hill Top] Loading %s model: %s... ", mdlformatstrings[format - 1], name);

	std::string fullPath = "system\\models\\";
	fullPath = fullPath + name + mdlformatfiletypes[format - 1];

	ModelInfo* mdl = new ModelInfo(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (mdl->getformat() != format) {
		PrintDebug("Failed!\n");
		delete mdl;
		return nullptr;
	}

	*info = mdl;

	PrintDebug("Done.\n");
	return mdl;
}