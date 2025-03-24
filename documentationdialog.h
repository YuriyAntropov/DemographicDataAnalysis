#ifndef DOCUMENTATIONDIALOG_H
#define DOCUMENTATIONDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

class DocumentationDialog : public QDialog {
    Q_OBJECT
public:
    explicit DocumentationDialog(QWidget *parent = nullptr);

private:
    void loadDocumentation();

    QTextEdit *textEdit;
    QPushButton *closeButton;
    QVBoxLayout *layout;
};

#endif // DOCUMENTATIONDIALOG_H
