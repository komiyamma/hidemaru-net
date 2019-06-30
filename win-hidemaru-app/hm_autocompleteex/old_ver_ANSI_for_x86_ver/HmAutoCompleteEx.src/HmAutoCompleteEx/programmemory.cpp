#include <fstream>
#include <algorithm>
#include <windows.h>
#include <vector>
#include <string.h>

using namespace std;

const IMAGE_DOS_HEADER *getMz(const unsigned char * const data) {
	return reinterpret_cast<const IMAGE_DOS_HEADER *>(data);
}

const IMAGE_NT_HEADERS32 *getPe(const unsigned char * const data, const IMAGE_DOS_HEADER &mz) {
	return reinterpret_cast<const IMAGE_NT_HEADERS32 *>(data + mz.e_lfanew);
}

const IMAGE_SECTION_HEADER *getSectionHeader(const unsigned char * const data, const IMAGE_DOS_HEADER &mz, const IMAGE_NT_HEADERS32 &pe) {
	return reinterpret_cast<const IMAGE_SECTION_HEADER *>(
		data
		+ mz.e_lfanew
		+ sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_OPTIONAL_HEADER32)
		+ pe.FileHeader.SizeOfOptionalHeader);
}


bool LoadImage(char *szPath, std::vector<unsigned char> &out) {
	ifstream ifs;
	ifs.open(szPath, std::ios::binary);
	if (!ifs.is_open()) {
		return false;
	}

	// MZ�w�b�_�[�̓ǂݍ���
	out.resize(sizeof(IMAGE_DOS_HEADER));
	ifs.read(reinterpret_cast<char*>(&out.front()), out.size());
	const IMAGE_DOS_HEADER *mz = getMz(&out.front());
	if (!ifs.good() || mz->e_magic != 0x5A4D) {
		return false;
	}

	// PE�w�b�_�[�̓ǂݍ���
	out.resize(mz->e_lfanew + sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_OPTIONAL_HEADER32));
	mz = getMz(&out.front());
	ifs.seekg(0, std::ios::beg);
	ifs.read(reinterpret_cast<char*>(&out.front()), out.size());
	const IMAGE_NT_HEADERS32 *pe = getPe(&out.front(), *mz);
	if (!ifs.good() || pe->Signature != 0x00004550) {
		return false;
	}

	// PE�I�v�V���i���w�b�_�[����Z�N�V�������X�g�܂ł̓ǂݍ���
	out.resize(
		mz->e_lfanew
		+ sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_OPTIONAL_HEADER32)
		+ pe->FileHeader.SizeOfOptionalHeader
		+ sizeof(IMAGE_SECTION_HEADER) * pe->FileHeader.NumberOfSections);
	mz = getMz(&out.front());
	pe = getPe(&out.front(), *mz);
	ifs.seekg(0, std::ios::beg);
	ifs.read(reinterpret_cast<char*>(&out.front()), out.size());
	if (!ifs.good()) {
		return false;
	}

	// �Z�N�V�����̓ǂݍ���
	const IMAGE_SECTION_HEADER *sectionList = getSectionHeader(&out.front(), *mz, *pe);
	for (unsigned int i = 0; i < pe->FileHeader.NumberOfSections; i++) {
		if (sectionList[i].Misc.VirtualSize == 0) {
			continue;
		}
		if (out.size() < sectionList[i].VirtualAddress + sectionList[i].Misc.VirtualSize) {
			out.resize(sectionList[i].VirtualAddress + sectionList[i].Misc.VirtualSize);
			mz = getMz(&out.front());
			pe = getPe(&out.front(), *mz);
			sectionList = getSectionHeader(&out.front(), *mz, *pe);
		}
		ifs.seekg(sectionList[i].PointerToRawData, std::ios::beg);
		ifs.read(reinterpret_cast<char*>(&out[sectionList[i].VirtualAddress]), (std::min)(sectionList[i].Misc.VirtualSize, sectionList[i].SizeOfRawData));
		if (!ifs.good()) {
			return false;
		}
	}
	return true;

}

int ProgramMemoryDstImageBase = 0;
int ProgramMemoryDstBaseOfData = 0;

int checkProgramMemoryImage(char*szPath) {

	std::vector<unsigned char> image;
	if (!LoadImage(szPath, image)) {
		// �C���[�W�ł͂Ȃ�
		return -1;
	}
	const unsigned char * const baseAddr = &image.front();
	const IMAGE_DOS_HEADER &mz = *getMz(baseAddr);
	const IMAGE_NT_HEADERS32 &pe = *getPe(baseAddr, mz);

	ProgramMemoryDstImageBase = pe.OptionalHeader.ImageBase + 0x1000;
	ProgramMemoryDstBaseOfData = ProgramMemoryDstImageBase + pe.OptionalHeader.BaseOfData + 0x1000;
	if (pe.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size == 0) {
		return false;
	}
	const IMAGE_DATA_DIRECTORY &loadConfigDir = pe.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
	const IMAGE_LOAD_CONFIG_DIRECTORY32 &loadConfig = *reinterpret_cast<const IMAGE_LOAD_CONFIG_DIRECTORY32 *>(baseAddr + loadConfigDir.VirtualAddress);
	if (loadConfig.Size < 0x5C) {
		return false;
	}
	if ((loadConfig.GuardFlags & IMAGE_GUARD_CF_FUNCTION_TABLE_PRESENT) == 0) {
		return false;
	}
	return true;

}


/*
* strstr��mem�ŁB�������̒��������̃o�C�g��̈ʒu�������o��
*/

void *memmem(const void *l, size_t l_len, const void *s, size_t s_len)
{
	register char *cur, *last;
	const char *cl = (const char *)l;
	const char *cs = (const char *)s;

	/* �Ӗ��Ȃ� */
	if (l_len == 0 || s_len == 0)
		return NULL;

	/* �������Ώۂƃo�C�g��ŁA�������Ώۂ̕����T�C�Y�������� */
	if (l_len < s_len)
		return NULL;

	/* �T�C�Y1��������memchr�Ɉڏ� == 1 */
	if (s_len == 1)
		return (void *)memchr(l, (int)*cs, l_len);

	/* �E�̃o�C�g��̒�������A�Ō�̈ʒu���v�Z */
	last = (char *)cl + l_len - s_len;

	// ���肭��
	for (cur = (char *)cl; cur <= last; cur++)
		if (cur[0] == cs[0] && memcmp(cur, cs, s_len) == 0)
			return cur;

	return NULL;
}

