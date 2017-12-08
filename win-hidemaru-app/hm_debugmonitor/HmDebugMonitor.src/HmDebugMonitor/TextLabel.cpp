/*
 * Copyright (c) 2016-2017 Akitsugu Komiyama
 * under the MIT License
 */

#include "TextLabel.h"
#include "RegexBox.h"

static HFONT hFont = NULL;

// ���K�\����M�t�B���^�[
static char *str = "�i���s���K�\���t��"; 
void CreateRegexBoxTextLabel(HWND hWnd) {

	int iClientWidth = 0;
	int iClientHeight = 0;

	RECT rc;
	GetClientRect(hWnd, &rc); // �N���C�A���g�̈�̃T�C�Y

	// ��]����N���C�A���g�̈�̃T�C�Y�����E�B���h�E�T�C�Y���v�Z
	iClientWidth = (rc.right - rc.left);
	iClientHeight = (rc.bottom - rc.top);


	HDC hdc;
    PAINTSTRUCT paint;

    hdc = BeginPaint(hWnd, &paint);
	hFont = SetLabelFont("�l�r �S�V�b�N", 14, 0);
	SelectObject( hdc, hFont );
	SetTextColor(hdc, RGB(30, 0, 0));

    TextOut(hdc, iClientWidth-RegexBoxWidth-5-130, 13, (LPCSTR)str, strlen(str));
    EndPaint(hWnd, &paint);
}

void DeleteRegexBoxTextLabel() {
	DeleteObject(hFont);
}

HFONT SetLabelFont(LPCTSTR face, int h, int angle) {
    HFONT hFont;
    hFont = CreateFont(h,    //�t�H���g����
        0,                    //������
        angle,                    //�e�L�X�g�̊p�x
        0,                    //�x�[�X���C���Ƃ����Ƃ̊p�x
        FW_REGULAR,            //�t�H���g�̏d���i�����j
        FALSE,                //�C�^���b�N��
        FALSE,                //�A���_�[���C��
        FALSE,                //�ł�������
        SHIFTJIS_CHARSET,    //�����Z�b�g
        OUT_DEFAULT_PRECIS,    //�o�͐��x
        CLIP_DEFAULT_PRECIS,//�N���b�s���O���x
        PROOF_QUALITY,        //�o�͕i��
        FIXED_PITCH | FF_MODERN,//�s�b�`�ƃt�@�~���[
        face);    //���̖�
    return hFont;
}