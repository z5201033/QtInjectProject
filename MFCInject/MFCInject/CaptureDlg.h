#pragma once
class CaptureDlg
{
public:
	CaptureDlg();
	~CaptureDlg();

	void updateCaptureDlgPos(HWND hwnd, int bShow = true);
	void updateCaptureDlgPos(int x, int y, int width, int height, int bShow = true);
	void showCaptureDlg();
	void hideCaptureDlg();

private:
	struct CaptureDlgPrivate* d;
};

