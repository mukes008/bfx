#include "bfxtcustommetadatawidget.h"

#include <QMessageBox>

BFXTCustomMetadataWidget::BFXTCustomMetadataWidget(QWidget* parent) : QWidget(parent)
{
    mainLayout = new QGridLayout(this);
    this->setLayout(mainLayout);

    jsonDataLineEdit = new QPlainTextEdit(this);
    okButton         = new QPushButton("OK", this);
    clearButton      = new QPushButton("Clear", this);
    viewAsTreeButton = new QPushButton("View as tree/text", this);
    jsonTreeView     = new JsonTreeView(this);
    jsonTreeModel    = new JsonTreeModel(nullptr, false, this);

    jsonTreeView->setModel(jsonTreeModel);

    jsonDataLineEdit->setPlaceholderText("Input json data here...");

    mainLayout->addWidget(okButton, 0, 0, 1, 1);
    mainLayout->addWidget(clearButton, 0, 1, 1, 1);
    mainLayout->addWidget(viewAsTreeButton, 0, 2, 1, 1);
    mainLayout->addWidget(jsonDataLineEdit, 1, 0, 1, 3);
    mainLayout->addWidget(jsonTreeView, 1, 0, 1, 3);

    connect(viewAsTreeButton, &QPushButton::clicked, this,
            &BFXTCustomMetadataWidget::slot_viewAsTreePressed);
    connect(clearButton, &QPushButton::clicked, this, &BFXTCustomMetadataWidget::slot_clearPressed);
    connect(okButton, &QPushButton::clicked, this, &BFXTCustomMetadataWidget::slot_okPressed);

    slot_setViewTextOrTree();
}

void BFXTCustomMetadataWidget::slot_okPressed() { emit sig_okPressed(); }

void BFXTCustomMetadataWidget::slot_clearPressed() { clearData(); }

void BFXTCustomMetadataWidget::slot_viewAsTreePressed()
{
    try {
        json_spirit::Object obj = getJsonObject();
        jsonTreeModel->setRoot(JsonTreeNode::ImportFromJson(obj));
    } catch (...) {
        QMessageBox::warning(this, "Parsing error",
                             "Failed to parse json data. Make sure you included valid json.");
        isTreeViewActive = false;
        slot_setViewTextOrTree();
        return;
    }

    if (isTreeViewActive) {
        isTreeViewActive = false;
        slot_setViewTextOrTree();
    } else {
        isTreeViewActive = true;
        slot_setViewTextOrTree();
    }
}

void BFXTCustomMetadataWidget::slot_setViewTextOrTree()
{
    jsonDataLineEdit->setVisible(!isTreeViewActive);
    jsonTreeView->setVisible(isTreeViewActive);
}

json_spirit::Object BFXTCustomMetadataWidget::getJsonObject() const
{
    json_spirit::Object resultObj;
    json_spirit::Array  metaArray;
    json_spirit::Value  readData;
    json_spirit::read_or_throw(jsonDataLineEdit->toPlainText().toUtf8().toStdString(), readData);

    resultObj.push_back(json_spirit::Pair("meta", readData));
    json_spirit::Pair p("userData", resultObj);

    return json_spirit::Object({p});
}

bool BFXTCustomMetadataWidget::isJsonEmpty() const
{
    return jsonDataLineEdit->toPlainText().trimmed().isEmpty();
}

bool BFXTCustomMetadataWidget::isJsonValid() const
{
    try {
        getJsonObject();
        return true;
    } catch (...) {
        return false;
    }
}

void BFXTCustomMetadataWidget::clearData()
{
    jsonDataLineEdit->clear();
    jsonTreeModel->setRoot(nullptr);
    isTreeViewActive = false;
    slot_setViewTextOrTree();
}
