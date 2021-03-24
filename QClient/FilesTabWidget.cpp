#include "FilesTabWidget.h"


FilesTabWidget::FilesTabWidget(Rd::Client cl, TransferModel* transferModel, QWidget *parent)
	: _client(cl), _transferModel(transferModel), TabItem(parent)
{
	ui.setupUi(this);

	setFocusPolicy(Qt::NoFocus);
	_leftModel = new LocalFilesModel(this);
	_rightModel = new RemoteFilesModel(cl, this);

	_left = new FilesWidget(_leftModel, this);
	_right = new FilesWidget(_rightModel, this);


	connect(_left, &FilesWidget::openUrl, [this](const std::filesystem::path& p) {
		QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdWString(p)));
		});

	connect(_left, &FilesWidget::copy, [this](const std::list<files::file_info_t>& l) {
		upload(l);
		});


	connect(_right, &FilesWidget::copy, [this](const std::list<files::file_info_t>& l) {
		download(l);
		});


	QHBoxLayout* hl = new QHBoxLayout(this);



	_btns = new QButtonGroup(this);
	QObject::connect(_btns, &QButtonGroup::idClicked, [this](int id) {
		if (id == 0)
		{
			_transferWidget->hide();
			_splitter->show();
		}
		if (id == 1)
		{
			_transferWidget->show();
			_splitter->hide();
		}

		});

	{
		QPushButton* b = new QPushButton("files", this);
		_btns->addButton(b, 0);
		b->setFocusPolicy(Qt::NoFocus);
		b->setCheckable(true);
		b->setChecked(true);
		hl->addWidget(b);
	}


	{
		QPushButton* b = new QPushButton("downloads", this);
		_btns->addButton(b, 1);
		b->setFocusPolicy(Qt::NoFocus);
		b->setCheckable(true);
		hl->addWidget(b);
	}



	_splitter = new QSplitter(this);
	_splitter->addWidget(_left);
	_splitter->addWidget(_right);


	_left->dir("/");
	_right->dir("/");


	_transferWidget = new TransferWidget(transferModel, this);


	//hl->setAlignment(Qt::AlignLeft);
	hl->setMargin(0);
	QWidget* w = new QWidget(this);
	w->setMaximumWidth(300);
	w->setLayout(hl);
	w->setMaximumHeight(30);
	
	QVBoxLayout* l = new QVBoxLayout(this);
	l->addWidget(w);
	l->addWidget(_splitter);
	l->addWidget(_transferWidget);
	_transferWidget->hide();
	l->setMargin(0);
	//l->setAlignment(Qt::AlignTop);
	setLayout(l);
	
}

FilesTabWidget::~FilesTabWidget()
{
}

void FilesTabWidget::getState(QSettings& sett)
{
}

void FilesTabWidget::setState(const QSettings& sett)
{
}




std::filesystem::path FilesTabWidget::replaceParent(const std::filesystem::path& p,
	const std::filesystem::path& old_p, const std::filesystem::path& new_p)
{
	std::filesystem::path res = new_p;
	auto it1 = old_p.begin();
	for (auto it = p.begin(); it != p.end(); it++)
	{
		if (it1 == old_p.end())
		{
			res = res.string() + "/" + it->string();
		}
		else
		{
			it1++;
		}

	}

	return res;
}




void FilesTabWidget::upload(const std::list<files::file_info_t>& l)
{
	for (auto& it : l)
	{
		if (it.type == files::file_type::directory_file)
		{
			_leftModel->dirRecursion(it.path, [this, it](const std::list<files::file_info_t>& arg)
				{
					for (auto& ff : arg)
					{
						std::filesystem::path p = replaceParent(ff.path, _left->path(), _right->path());
						if (ff.type == files::file_type::directory_file)
						{
							_rightModel->mkdir(p);
						}
						else
						{
							_transferModel->post(_client, ff.path, p);
						}
					}
				});
		}
		else
		{
			_transferModel->post(_client, it.path, _right->path().string() + "/" + it.path.filename().string());
		}
	}
}

void FilesTabWidget::download(const std::list<files::file_info_t>& l)
{
	for (auto it : l)
	{
		if (it.type == files::file_type::directory_file)
		{
			_rightModel->dirRecursion(it.path, [this, it](const std::list<files::file_info_t>& arg)
				{
					for (auto& ff : arg)
					{
						std::filesystem::path p = replaceParent(ff.path, _right->path(), _left->path());
						if (ff.type == files::file_type::directory_file)
						{
							_leftModel->mkdir(p);
						}
						else
						{
							_transferModel->get(_client, p, ff.path);
						}
					}
				});
		}
		else
		{
			_transferModel->get(_client, _left->path().string() + "/" + it.path.filename().string(), it.path);
		}
	}
}
