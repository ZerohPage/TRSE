#include "formhexedit.h"
#include "ui_formhexedit.h"

FormHexEdit::FormHexEdit(QWidget *parent) :
    TRSEDocument(parent),
    ui(new Ui::FormHexEdit)
{
    ui->setupUi(this);
    m_programEndingType = "bin";
}

FormHexEdit::~FormHexEdit()
{
    delete ui;
}

bool FormHexEdit::Load(QString filename) {
    m_data = Util::loadBinaryFile(filename);
    ui->scrollArea->setData(new HexView::DataStorageArray(m_data));
    return true;
}

void FormHexEdit::Save(QString filename)
{
    m_data = ui->scrollArea->m_pdata->m_data;
    Util::SaveByteArray(m_data,filename);

}

void FormHexEdit::InitDocument(WorkerThread *t, QSharedPointer<CIniFile> ini, QSharedPointer<CIniFile> iniProject) {
    TRSEDocument::InitDocument(t,ini,iniProject);
    ui->scrollArea->m_doc = this;
    //ui->sc

    m_colors.Load(Util::GetSystemPrefix() + "themes/" + m_iniFile->getString("theme"));
    ui->scrollArea->m_colors = &m_colors;
}


void FormHexEdit::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->scrollArea->BYTES_PER_LINE = arg1.toInt();
    ui->scrollArea->update();
    ui->scrollArea->viewport()->update();

}
