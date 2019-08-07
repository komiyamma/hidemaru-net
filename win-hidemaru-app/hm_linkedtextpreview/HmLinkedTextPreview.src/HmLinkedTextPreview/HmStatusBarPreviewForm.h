/*
 * Copyright(C) 2017 Akitsugu Komiyama
 * under the MIT License
 */ 

#pragma once

#include <windows.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;

extern void OutputDebugStream(const wchar_t *format, ...);

public ref class HmStatsusBarPreviewForm : public Form
{
public:
	static HmStatsusBarPreviewForm^ f; // �������g�̒u����
	static void SetBColor(Color color) {
		bcolor = color;
	}

private:
	static String^ filename; // ���݂̃t�@�C����
	HWND hWndStatusBar;      // �X�e�[�^�X�E�B���h�E��
	static Color bcolor = Color::WhiteSmoke;
public:
	String^ GetFileName() {
		return filename;
	}
private:
	int nTickRemainCnt;      // �`�撲���p�̃J�E���^
	String^ strTarget;
	int LoadImageCheckedStatus = 0;			// �f�[�^�����邩�ǂ����`�F�b�N����
	PictureBox^ pb;

	bool isShouldBeShow = false;
public:
	bool IsSouldBeShow() {
		return isShouldBeShow;
	}
public:
	HmStatsusBarPreviewForm(HWND hWnd, String^ target)
	{
		LoadImageCheckedStatus = 0;
		isShouldBeShow = false;
		this->hWndStatusBar = hWnd;
		this->filename = "aaa";
		this->strTarget = target;

		this->BackColor = System::Drawing::Color(this->bcolor);

		this->StartPosition = FormStartPosition::Manual;
		this->Paint += gcnew PaintEventHandler(this, &HmStatsusBarPreviewForm::form_Paint);

		this->pb = gcnew PictureBox();
		this->BackColor = bcolor;
		SetFormAttr();

		MovePosition();
	}

public:
	Image^ currentImage;
	void form_Paint(Object^o, PaintEventArgs^ e) {
		if (currentImage != nullptr)
		{
			//DrawImage���\�b�h�ŉ摜��`�悷��
			e->Graphics->DrawImage(currentImage,
				0, 0, currentImage->Width, currentImage->Height);
		}
	}

	bool IsPictureFile(String^ strTarget) {
		if (strTarget->Contains(".png") ||
			strTarget->Contains(".gif") ||
			strTarget->Contains(".jpg") || strTarget->Contains(".jpeg") ||
			//			strTarget->Contains(".ico") ||
			strTarget->Contains(".bmp")) {
			return true;
		}
		else {
			return false;
		}
	}

	static System::Drawing::Image ^CreateImage(String ^filename)
	{
		try {
			System::IO::FileStream ^fs = gcnew System::IO::FileStream(filename, System::IO::FileMode::Open, System::IO::FileAccess::Read);
			System::Drawing::Image ^img = System::Drawing::Image::FromStream(fs);
			fs->Close();

			return img;
		}
		catch (Exception^) {}

		return nullptr;
	}

	// �Ĉړ�
	void MovePosition()
	{
		try {
			if (LoadImageCheckedStatus == 0) {
				try {
					if (IsPictureFile(strTarget)) {
						if (System::IO::File::Exists(strTarget)) {
							currentImage = CreateImage(strTarget);

							pb->Image = currentImage;
							pb->Width = currentImage->Width;
							pb->Height = currentImage->Height;

							auto color = bcolor;
							pb->BackColor = Color::Transparent;

							LoadImageCheckedStatus = 1;
							isShouldBeShow = true;
						}
					}
				}
				catch (Exception^ e) {
				}
			}
			// �}�E�X���W�̎擾
			POINT po;
			GetCursorPos(&po);
			// OutputDebugStream(L"%d, %d\n", po.x, po.y);
			this->Left = po.x + 50;
			this->Top = po.y + 50;
			if (currentImage) {
				this->Width = currentImage->Width;
				this->Height = currentImage->Height;

				if (LoadImageCheckedStatus <= 3) {
					this->BackColor = bcolor;
					this->Refresh();
					LoadImageCheckedStatus++;
				}
			}
		}
		catch (Exception^) {

		}
	}

protected:
	// �t�H�[���̊O���ɉ����Ďl�p��`��
	void DrawRectangle()
	{
		Graphics^ g = this->CreateGraphics();
		Pen^ pen = gcnew Pen(::Color::Black, 3);
		g->DrawRectangle(pen, 0, 0, this->Width - 1, this->Height - 1);
		delete pen;
		delete g;
	}


	void SetFormAttr() {
		//�^�C�g���o�[������
		this->ControlBox = false;
		this->Text = "";
		this->FormBorderStyle = ::FormBorderStyle::None;
	}

	// �t�H�[���\�����ɃA�N�e�B�u�ɂȂ�Ȃ��悤�ɂ���
	property bool ShowWithoutActivation
	{
		virtual bool get() override
		{
			return true;
		}
	}

	// ���̃t�H�[�����N���b�N�Ȃǂ��ꂽ���ɃA�N�e�B�u�ɂȂ�Ȃ��悤�ɂ���B
	virtual void WndProc(Message %m) override
	{
		if (m.Msg == WM_MOUSEACTIVATE)
		{
			m.Result = (IntPtr)MA_NOACTIVATE;
			return;
		}

		Form::WndProc(m);
	}

};

