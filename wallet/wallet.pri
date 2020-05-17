# Input
DEPENDPATH += . json qt
HEADERS += qt/bitcoingui.h \
    qt/transactiontablemodel.h \
    qt/addresstablemodel.h \
    qt/optionsdialog.h \
    qt/coincontroldialog.h \
    qt/coincontroltreewidget.h \
    qt/sendcoinsdialog.h \
    qt/addressbookpage.h \
    qt/signverifymessagedialog.h \
    qt/aboutdialog.h \
    qt/editaddressdialog.h \
    qt/bitcoinaddressvalidator.h \
    alert.h \
    addrman.h \
    base58.h \
    bignum.h \
    checkpoints.h \
    compat.h \
    coincontrol.h \
    sync.h \
    util.h \
    hash.h \
    uint256.h \
    kernel.h \
    scrypt.h \
    pbkdf2.h \
    zerocoin/Accumulator.h \
    zerocoin/AccumulatorProofOfKnowledge.h \
    zerocoin/Coin.h \
    zerocoin/CoinSpend.h \
    zerocoin/Commitment.h \
    zerocoin/ParamGeneration.h \
    zerocoin/Params.h \
    zerocoin/SerialNumberSignatureOfKnowledge.h \
    zerocoin/SpendMetaData.h \
    zerocoin/ZeroTest.h \
    zerocoin/Zerocoin.h \
    serialize.h \
    main.h \
    miner.h \
    net.h \
    key.h \
    db.h \
    txdb.h \
    walletdb.h \
    script.h \
    init.h \
    hash.h \
    bloom.h \
    mruset.h \
    json/json_spirit_writer_template.h \
    json/json_spirit_writer.h \
    json/json_spirit_value.h \
    json/json_spirit_utils.h \
    json/json_spirit_stream_reader.h \
    json/json_spirit_reader_template.h \
    json/json_spirit_reader.h \
    json/json_spirit_error_position.h \
    json/json_spirit.h \
    qt/clientmodel.h \
    qt/guiutil.h \
    qt/transactionrecord.h \
    qt/guiconstants.h \
    qt/optionsmodel.h \
    qt/monitoreddatamapper.h \
    qt/transactiondesc.h \
    qt/transactiondescdialog.h \
    qt/bitcoinamountfield.h \
    wallet.h \
    keystore.h \
    qt/transactionfilterproxy.h \
    qt/transactionview.h \
    qt/walletmodel.h \
    bitcoinrpc.h \
    qt/overviewpage.h \
    qt/ui_overviewpage.h \
    qt/ui_qrcodedialog.h \
    qt/bfxtsummary.h \
    qt/ui_bfxtsummary.h \
    qt/ui_sendcoinsdialog.h \
    qt/ui_coincontroldialog.h \
    qt/ui_sendcoinsdialog.h \
    qt/ui_addressbookpage.h \
    qt/ui_signverifymessagedialog.h \
    qt/ui_aboutdialog.h \
    qt/ui_editaddressdialog.h \
    qt/ui_transactiondescdialog.h \
    qt/ui_sendcoinsentry.h \
    qt/ui_askpassphrasedialog.h \
    qt/ui_rpcconsole.h \
    qt/ui_optionsdialog.h \
    qt/csvmodelwriter.h \
    crypter.h \
    qt/sendcoinsentry.h \
    qt/qvalidatedlineedit.h \
    qt/bitcoinunits.h \
    qt/qvaluecombobox.h \
    qt/askpassphrasedialog.h \
    qt/bfxupdatedialog.h \
    protocol.h \
    qt/notificator.h \
    qt/qtipcserver.h \
    allocators.h \
    ui_interface.h \
    qt/rpcconsole.h \
    qt/ClickableLabel.h \
    version.h \
    netbase.h \
    clientversion.h \
    threadsafety.h \
    bfxupdater.h \
    bfxversion.h \
    bfxreleaseinfo.h \
    curltools.h \
    qt/messageboxwithtimer.h \
    qt/bfxt/bfxttokenlistmodel.h \
    qt/bfxt/bfxttokenlistfilterproxy.h \
    bfxt/bfxttokenmetadata.h \
    bfxt/bfxtwallet.h \
    qt/bfxt/bfxttokenlistitemdelegate.h \
    ThreadSafeHashMap.h \
    qt/bfxtsendtokensfeewidget.h \
    bfxt/bfxtscript_burn.h \
    bfxt/bfxttokenminimalmetadata.h \
    qt/bfxt/bfxtlistelementtokendata.h \
    bfxt/bfxtsendtxdata.h \
    NetworkForks.h \
    bfxt/bfxtv1_issuance_static_data.h \
    qt/json/AbstractTreeNode.h  \
    qt/json/JsonNewNodeDialog.h \
    qt/json/JsonTreeModel.h     \
    qt/json/JsonTreeNode.h      \
    qt/json/JsonTreeView.h      \
    qt/bfxt/bfxtcreatemetadatadialog.h \
    qt/bfxt/bfxtmetadatapairwidget.h \
    qt/bfxt/bfxtcustommetadatawidget.h \
    qt/bfxt/bfxtmetadatapairswidget.h \
    qt/json/BFXTMetadataViewer.h \
    SerializationTester.h \
    blockindexcatalog.h   \
    blockindex.h          \
    outpoint.h            \
    inpoint.h             \
    block.h               \
    transaction.h         \
    globals.h             \
    diskblockindex.h      \
    disktxpos.h           \
    txindex.h             \
    txin.h                \
    txout.h               \
    txmempool.h           \
    merkletx.h            \
    blocklocator.h        \
    udaddress.h           \
    qt/bfxt/issuenewbfxttokendialog.h \
    crypto_highlevel.h



HEADERS +=                 \
    bfxt/bfxttools.h       \
    bfxt/bfxtinpoint.h     \
    bfxt/bfxtoutpoint.h    \
    bfxt/bfxttransaction.h \
    bfxt/bfxttxin.h        \
    bfxt/bfxttxout.h       \
    bfxt/bfxttokentxdata.h \
    bfxt/bfxtapicalls.h    \
    bfxt/bfxtsendtokensonerecipientdata.h \
    bfxt/bfxtscript.h      \
    bfxt/bfxtscript_issuance.h \
    bfxt/bfxtscript_transfer.h




SOURCES += qt/bitcoin.cpp \
    qt/bitcoingui.cpp \
    qt/transactiontablemodel.cpp \
    qt/addresstablemodel.cpp \
    qt/optionsdialog.cpp \
    qt/sendcoinsdialog.cpp \
    qt/coincontroldialog.cpp \
    qt/coincontroltreewidget.cpp \
    qt/addressbookpage.cpp \
    qt/signverifymessagedialog.cpp \
    qt/aboutdialog.cpp \
    qt/editaddressdialog.cpp \
    qt/bitcoinaddressvalidator.cpp \
    alert.cpp \
    version.cpp \
    sync.cpp \
    util.cpp \
    hash.cpp \
    netbase.cpp \
    key.cpp \
    script.cpp \
    main.cpp \
    miner.cpp \
    init.cpp \
    net.cpp \
    bloom.cpp \
    checkpoints.cpp \
    addrman.cpp \
    db.cpp \
    walletdb.cpp \
    qt/clientmodel.cpp \
    qt/guiutil.cpp \
    qt/transactionrecord.cpp \
    qt/optionsmodel.cpp \
    qt/monitoreddatamapper.cpp \
    qt/transactiondesc.cpp \
    qt/transactiondescdialog.cpp \
    qt/bitcoinstrings.cpp \
    qt/bitcoinamountfield.cpp \
    wallet.cpp \
    keystore.cpp \
    qt/transactionfilterproxy.cpp \
    qt/transactionview.cpp \
    qt/walletmodel.cpp \
    bitcoinrpc.cpp \
    rpcdump.cpp \
    rpcnet.cpp \
    rpcmining.cpp \
    rpcwallet.cpp \
    rpcblockchain.cpp \
    rpcrawtransaction.cpp \
    qt/overviewpage.cpp \
    qt/bfxtsummary.cpp \
    qt/csvmodelwriter.cpp \
    crypter.cpp \
    qt/sendcoinsentry.cpp \
    qt/qvalidatedlineedit.cpp \
    qt/bitcoinunits.cpp \
    qt/qvaluecombobox.cpp \
    qt/askpassphrasedialog.cpp \
    protocol.cpp \
    qt/notificator.cpp \
    qt/qtipcserver.cpp \
    qt/rpcconsole.cpp \
    qt/ClickableLabel.cpp \
    qt/bfxupdatedialog.cpp \
    qt/messageboxwithtimer.cpp \
    noui.cpp \
    kernel.cpp \
    scrypt-arm.S \
    scrypt-x86.S \
    scrypt-x86_64.S \
    scrypt.cpp \
    pbkdf2.cpp \
    zerocoin/Accumulator.cpp \
    zerocoin/AccumulatorProofOfKnowledge.cpp \
    zerocoin/Coin.cpp \
    zerocoin/CoinSpend.cpp \
    zerocoin/Commitment.cpp \
    zerocoin/ParamGeneration.cpp \
    zerocoin/Params.cpp \
    zerocoin/SerialNumberSignatureOfKnowledge.cpp \
    zerocoin/SpendMetaData.cpp \
    bfxupdater.cpp \
    bfxversion.cpp \
    json/json_spirit_value.cpp \
    json/json_spirit_reader.cpp \
    json/json_spirit_writer.cpp \
    bfxreleaseinfo.cpp \
    zerocoin/ZeroTest.cpp \
    curltools.cpp \
    qt/bfxt/bfxttokenlistmodel.cpp \
    qt/bfxt/bfxttokenlistfilterproxy.cpp \
    bfxt/bfxttokenmetadata.cpp \
    bfxt/bfxtwallet.cpp \
    qt/bfxt/bfxttokenlistitemdelegate.cpp \
    ThreadSafeHashMap.cpp \
    bfxt/bfxtsendtokensonerecipientdata.cpp \
    qt/bfxtsendtokensfeewidget.cpp \
    bfxt/bfxtscript_burn.cpp \
    bfxt/bfxttokenminimalmetadata.cpp \
    bfxt/bfxtsendtxdata.cpp \
    NetworkForks.cpp \
    bfxt/bfxtv1_issuance_static_data.cpp \
    qt/json/AbstractTreeNode.cpp  \
    qt/json/JsonNewNodeDialog.cpp \
    qt/json/JsonTreeModel.cpp     \
    qt/json/JsonTreeNode.cpp      \
    qt/json/JsonTreeView.cpp      \
    qt/json/BFXTMetadataViewer.cpp \
    qt/bfxt/bfxtcreatemetadatadialog.cpp \
    qt/bfxt/bfxtmetadatapairwidget.cpp \
    qt/bfxt/bfxtcustommetadatawidget.cpp \
    qt/bfxt/bfxtmetadatapairswidget.cpp \
    SerializationTester.cpp \
    blockindexcatalog.cpp \
    blockindex.cpp        \
    outpoint.cpp          \
    inpoint.cpp           \
    block.cpp             \
    transaction.cpp       \
    globals.cpp           \
    diskblockindex.cpp    \
    disktxpos.cpp         \
    txindex.cpp           \
    txin.cpp              \
    txout.cpp             \
    txmempool.cpp         \
    merkletx.cpp          \
    blocklocator.cpp      \
    udaddress.cpp         \
    qt/bfxt/issuenewbfxttokendialog.cpp \
    crypto_highlevel.cpp


SOURCES +=                   \
    bfxt/bfxttools.cpp       \
    bfxt/bfxtinpoint.cpp     \
    bfxt/bfxtoutpoint.cpp    \
    bfxt/bfxttransaction.cpp \
    bfxt/bfxttxin.cpp        \
    bfxt/bfxttxout.cpp       \
    bfxt/bfxttokentxdata.cpp \
    bfxt/bfxtapicalls.cpp    \
    bfxt/bfxtscript.cpp      \
    bfxt/bfxtscript_issuance.cpp \
    bfxt/bfxtscript_transfer.cpp


contains(BFX_REST, 1) {
    HEADERS += bfxrest.h
    SOURCES += bfxrest.cpp
}

contains(USE_QRCODE, 1) {
HEADERS += qt/qrcodedialog.h
SOURCES += qt/qrcodedialog.cpp
FORMS += qt/forms/qrcodedialog.ui
}
