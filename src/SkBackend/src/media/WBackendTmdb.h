//=================================================================================================
/*
    Copyright (C) 2015-2017 Sky kit authors united with omega. <http://omega.gg/about>

    Author: Benjamin Arnaud. <http://bunjee.me> <bunjee@omega.gg>

    This file is part of the SkBackend module of Sky kit.

    - GNU General Public License Usage:
    This file may be used under the terms of the GNU General Public License version 3 as published
    by the Free Software Foundation and appearing in the LICENSE.md file included in the packaging
    of this file. Please review the following information to ensure the GNU General Public License
    requirements will be met: https://www.gnu.org/licenses/gpl.html.
*/
//=================================================================================================

#ifndef WBACKENDTMDB_H
#define WBACKENDTMDB_H

// Sk includes
#include <WBackendNet>

#ifndef SK_NO_BACKENDTMDB

// Forward declarations
class WBackendTmdbPrivate;

class SK_BACKEND_EXPORT WBackendTmdb : public WBackendNet
{
    Q_OBJECT

public:
    WBackendTmdb();

public: // WBackendNet implementation
    /* Q_INVOKABLE virtual */ QString getId   () const;
    /* Q_INVOKABLE virtual */ QString getTitle() const;

public: // WBackendNet reimplementation
    /* Q_INVOKABLE virtual */ bool isSearchCover() const;

    /* Q_INVOKABLE virtual */ bool checkValidUrl(const QUrl & url) const;

    /* Q_INVOKABLE virtual */ bool checkCover(const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetQuery createQuery(const QString & method,
                                 const QString & label, const QString & q) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */
    WBackendNetTrack extractTrack(const QByteArray & data, const WBackendNetQuery & query) const;

    //---------------------------------------------------------------------------------------------

    /* Q_INVOKABLE virtual */ void applyTrack(const WBackendNetQuery & query,
                                              const WBackendNetTrack & track);

private:
    W_DECLARE_PRIVATE(WBackendTmdb)
};

#endif // SK_NO_BACKENDTMDB
#endif // WBACKENDTMDB_H
