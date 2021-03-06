#ifndef ISSUENEWBFXTTOKENDIALOG_H
#define ISSUENEWBFXTTOKENDIALOG_H

#include "bfxtcreatemetadatadialog.h"
#include <QCheckBox>
#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <unordered_set>

#include "bfxt/bfxtscript.h"
#include <QValidator>

#include "json/json_spirit.h"

class IssueNewBFXTTokenDialog;
class CCoinControl;
class CoinControlDialog;
class WalletModel;

class BFXTTokenSymbolValidator : public QValidator
{
    std::unordered_set<std::string> alreadyIssuedTokenSymbols;

    IssueNewBFXTTokenDialog& dialog;

public:
    State validate(QString& input, int& /*pos*/) const override;
    void  setAlreadyIssuedTokenSymbols(const std::unordered_set<std::string>& tokenSymbols);
    bool  tokenWithSymbolAlreadyIssued(const std::string& tokenSymbol);
    BFXTTokenSymbolValidator(IssueNewBFXTTokenDialog& isseNewBFXTDialog, QObject* parent = Q_NULLPTR);
};

class BFXTTokenAmountValidator : public QValidator
{
public:
    BFXTTokenAmountValidator(QObject* parent = 0);
    State validate(QString& input, int& /*pos*/) const override;
};

class IssueNewBFXTTokenDialog : public QDialog
{
    Q_OBJECT

    QGridLayout* mainLayout;

    QLabel*                   tokenSymbolLabel;
    QLineEdit*                tokenSymbolLineEdit;
    QLabel*                   tokenSymbolErrorLabel;
    QLabel*                   issuerLabel;
    QLineEdit*                issuerLineEdit;
    QLabel*                   tokenNameLabel;
    QLineEdit*                tokenNameLineEdit;
    QLabel*                   amountLabel;
    QLineEdit*                amountLineEdit;
    QLabel*                   iconUrlLabel;
    QLabel*                   iconUrlMimeTypeLabel;
    QLineEdit*                iconUrlLineEdit;
    BFXTCreateMetadataDialog* metadataDialog;
    QPushButton*              editMetadataButton;
    QPushButton*              issueButton;
    QPushButton*              cancelButton;
    QPushButton*              clearButton;
    QLabel*                   targetAddressLabel;
    QLineEdit*                targetAddressLineEdit;
    QCheckBox*                changeAddressCheckbox;
    QLineEdit*                changeAddressLineEdit;
    QLabel*                   costLabel;
    QFrame*                   paymentSeparator;

    BFXTTokenSymbolValidator* tokenSymbolValidator;

    CoinControlDialog* coinControlDialog;
    QPushButton*       coinControlButton;

    WalletModel* walletModel = nullptr;

    void createWidgets();

public:
    IssueNewBFXTTokenDialog(QWidget* parent = 0);
    void clearData();
    void validateInput() const;
    void setAlreadyIssuedTokensSymbols(const std::unordered_set<std::string>& tokenSymbols);

    void setTokenSymbolValidatorErrorString(const QString& str);

    void setWalletModel(WalletModel* WalletModelPtr);

    json_spirit::Value getIssuanceMetadata() const;

private slots:
    void slot_clearData();
    void slot_modifyChangeAddressColor();
    void slot_modifyTargetAddressColor();
    void slot_changeAddressCheckboxToggled(bool checked);
    void slot_doIssueToken();
    void slot_iconUrlChanged(const QString& url);
    void slot_coinControlButtonClicked();
};

#endif // ISSUENEWBFXTTOKENDIALOG_H
