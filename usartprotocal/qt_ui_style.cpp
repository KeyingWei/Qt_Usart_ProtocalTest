#include <QApplication>
#include <QFile>
#include <QString>
#include "qt_ui_style.h"
/*==============================================================================
���ܣ���������qss�ļ�
�������ļ�·��
���أ���
==============================================================================*/
void QT_UI_Style::style_set(const QString &style)
{
    QFile qss(style);
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();
}
