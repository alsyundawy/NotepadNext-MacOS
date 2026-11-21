#pragma once
/*
 * This file is part of NotepadNext-MacOS.
 * Adapted and ported from SparkNotePad.
 *
 * Copyright 2026 Harry Dertin Sutisna Alsyundawy <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUTION <https://alsyundawy.com>
 *
 * NotepadNext-MacOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <QDialog>
#include <QString>

class MainWindow;
class QTabWidget;
class QPlainTextEdit;
class QLineEdit;
class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QProgressBar;

class DeveloperToolsDialog : public QDialog {
    Q_OBJECT

public:
    explicit DeveloperToolsDialog(MainWindow *mainWindow, int initialTab = 0, QWidget *parent = nullptr);
    ~DeveloperToolsDialog() override = default;

    void setCurrentTab(int index);

private slots:
    void onLoadFromEditor();
    void onApplyToEditor();
    void onCopyOutput();

    // Tab 0: Hash
    void onComputeHash();
    void onCompareHash();
    void onBrowseFileForHash();

    // Tab 1: MIME
    void onBase64Encode();
    void onBase64Decode();
    void onUrlEncode();
    void onUrlDecode();
    void onHtmlEntityEncode();
    void onHtmlEntityDecode();
    void onQuotedPrintableEncode();
    void onQuotedPrintableDecode();

    // Tab 2: Converter
    void onConvertRadix();
    void onAsciiToHex();
    void onHexToAscii();
    void onCaseTitle();
    void onCaseInvert();
    void onCaseSentence();

    // Tab 3: Encryption
    void onEvaluatePassword();
    void onHtmlEncrypt();
    void onHtmlDecrypt();

    // Tab 4: Developer Tools
    void onFormatJson();
    void onMinifyJson();
    void onFormatXml();
    void onMinifyXml();
    void onFormatHtml();
    void onMinifyHtml();
    void onFormatCss();
    void onMinifyCss();
    void onFormatJs();
    void onMinifyJs();
    void onParseJwt();
    void onGenerateUuid();
    void onCurrentEpoch();

private:
    void setupUi();
    QWidget *createHashTab();
    QWidget *createMimeTab();
    QWidget *createConverterTab();
    QWidget *createCryptoTab();
    QWidget *createDevToolsTab();

    MainWindow *m_mainWindow = nullptr;
    QTabWidget *m_tabWidget = nullptr;

    // Shared input/output
    QPlainTextEdit *m_inputEdit = nullptr;
    QPlainTextEdit *m_outputEdit = nullptr;
    QLabel *m_statusLabel = nullptr;

    // Hash tab controls
    QComboBox *m_comboHashAlgo = nullptr;
    QLineEdit *m_compareHashEdit = nullptr;
    QLabel *m_hashCompareResult = nullptr;
    QLineEdit *m_filePathEdit = nullptr;

    // Converter tab controls
    QComboBox *m_comboFromRadix = nullptr;
    QComboBox *m_comboToRadix = nullptr;
    QCheckBox *m_chkUppercaseHex = nullptr;

    // Crypto tab controls
    QLineEdit *m_cryptoPassphrase = nullptr;
    QComboBox *m_comboHtmlEncryptMethod = nullptr;
    QProgressBar *m_passwordScoreBar = nullptr;
    QLabel *m_passwordStrengthLabel = nullptr;

    // DevTools controls
    QSpinBox *m_spinJsonIndent = nullptr;
    QCheckBox *m_chkUuidUpper = nullptr;
    QCheckBox *m_chkUuidBraces = nullptr;
    QCheckBox *m_chkUuidDashes = nullptr;
};
