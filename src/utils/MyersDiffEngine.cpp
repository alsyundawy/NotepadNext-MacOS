/*
 * NotepadNext-MacOS - A cross-platform reimplementation of Notepad++ for macOS
 * Copyright (C) 2026 ALSYUNDAWY IT SOLUSTION
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "MyersDiffEngine.h"
#include <QLatin1Char>
#include <QStringList>
#include <algorithm>
#include <vector>

MyersDiffEngine::DiffResult MyersDiffEngine::compare(const QString &textA, const QString &textB) {
    DiffResult result;
    const QStringList linesA = textA.split(QLatin1Char('\n'));
    const QStringList linesB = textB.split(QLatin1Char('\n'));

    result.lines = computeLcs(linesA, linesB);

    using enum MyersDiffEngine::DiffOp;
    for (const auto &line : result.lines) {
        if (line.op == Insert) {
            result.additions++;
            result.hasDifferences = true;
        } else if (line.op == Delete) {
            result.deletions++;
            result.hasDifferences = true;
        } else if (line.op == Modify) {
            result.modifications++;
            result.hasDifferences = true;
        }
    }

    return result;
}

QVector<MyersDiffEngine::DiffLine> MyersDiffEngine::computeLcs(const QStringList &linesA, const QStringList &linesB) {
    const auto n = static_cast<int>(linesA.size());
    const auto m = static_cast<int>(linesB.size());

    std::vector dp(n + 1, std::vector(m + 1, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (linesA[i] == linesB[j]) {
                dp[i + 1][j + 1] = dp[i][j] + 1;
            } else {
                dp[i + 1][j + 1] = std::max(dp[i + 1][j], dp[i][j + 1]);
            }
        }
    }

    QVector<DiffLine> rawDiff;
    int i = n;
    int j = m;

    using enum MyersDiffEngine::DiffOp;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && linesA[i - 1] == linesB[j - 1]) {
            DiffLine dl;
            dl.op = Equal;
            dl.lineA = i - 1;
            dl.lineB = j - 1;
            dl.textA = linesA[i - 1];
            dl.textB = linesB[j - 1];
            rawDiff.append(dl);
            --i;
            --j;
        } else if (j > 0 && (i == 0 || dp[i][j - 1] >= dp[i - 1][j])) {
            DiffLine dl;
            dl.op = Insert;
            dl.lineA = -1;
            dl.lineB = j - 1;
            dl.textA = QString();
            dl.textB = linesB[j - 1];
            rawDiff.append(dl);
            --j;
        } else if (i > 0 && (j == 0 || dp[i][j - 1] < dp[i - 1][j])) {
            DiffLine dl;
            dl.op = Delete;
            dl.lineA = i - 1;
            dl.lineB = -1;
            dl.textA = linesA[i - 1];
            dl.textB = QString();
            rawDiff.append(dl);
            --i;
        }
    }

    std::ranges::reverse(rawDiff);

    QVector<DiffLine> processed;
    int idx = 0;
    while (idx < rawDiff.size()) {
        if (idx + 1 < rawDiff.size() && rawDiff[idx].op == Delete && rawDiff[idx + 1].op == Insert) {
            DiffLine mod;
            mod.op = Modify;
            mod.lineA = rawDiff[idx].lineA;
            mod.lineB = rawDiff[idx + 1].lineB;
            mod.textA = rawDiff[idx].textA;
            mod.textB = rawDiff[idx + 1].textB;
            processed.append(mod);
            idx += 2;
        } else {
            processed.append(rawDiff[idx]);
            idx++;
        }
    }

    return processed;
}
