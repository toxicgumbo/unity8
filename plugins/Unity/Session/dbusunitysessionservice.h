﻿/*
 * Copyright (C) 2014 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DBUSUNITYSESSIONSERVICE_H
#define DBUSUNITYSESSIONSERVICE_H

#include <QDBusObjectPath>

#include "unitydbusobject.h"

typedef QList<QDBusObjectPath> QDbusList;
Q_DECLARE_METATYPE(QList<QDBusObjectPath>)

/**
 * DBusUnitySessionService provides com.canonical.Unity.Session dbus
 * interface.
 *
 * com.canonical.Unity.Session interface provides public methods
 * and signals to handle Logout/Reboot/Shutdown.
 */
class DBusUnitySessionService : public UnityDBusObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Session")

public:
    DBusUnitySessionService();
    ~DBusUnitySessionService() = default;

    // For use in QML. Javascript doesn't accept functions beginning with capital letters
    Q_INVOKABLE void logout() { Logout(); }
    Q_INVOKABLE void reboot() { Reboot(); }
    Q_INVOKABLE void shutdown() { Shutdown(); }
    Q_INVOKABLE void endSession() { EndSession(); }

Q_SIGNALS:
    /**
     * LogoutRequested signal
     *
     * This signal is emitted when some applications request the system to
     * logout.
     * @param have_inhibitors if there are any special running applications
     *        which inhibit the logout.
     */
    Q_SCRIPTABLE void LogoutRequested(bool have_inhibitors);

    /**
     * RebootRequested signal
     *
     * This signal is emitted when some applications request the system to
     * reboot.
     * @param have_inhibitors if there are any special running applications
     *        which inhibit the reboot.
     */
    Q_SCRIPTABLE void RebootRequested(bool have_inhibitors);

    /**
     * ShutdownRequested signal
     *
     * This signal is emitted when some applications request the system to
     * shutdown.
     * @param have_inhibitors if there are any special running applications
     *        which inhibit the shutdown.
     */
    Q_SCRIPTABLE void ShutdownRequested(bool have_inhibitors);

    /**
     * LogoutReady signal
     *
     * This signal is emitted when all the apps are closed. And the system
     * is safe to logout.
     */
    Q_SCRIPTABLE void LogoutReady();

    Q_SCRIPTABLE void LockRequested();

public Q_SLOTS:
    /**
     * Logout the system.
     *
     * This method directly logs out the system without user's confirmation.
     * Ordinary applications should avoid calling this method. Please call
     * RequestLogout() to ask the user to decide logout or not.
     */
    Q_SCRIPTABLE void Logout();

    /**
     * Reboot the system.
     *
     * This method directly reboots the system without user's confirmation.
     * Ordinary applications should avoid calling this method. Please call
     * RequestReboot() to ask the user to decide reboot or not.
     */
    Q_SCRIPTABLE void Reboot();

    /**
     * Shutdown the system.
     *
     * This method directly shuts down the system without user's confirmation.
     * Ordinary applications should avoid calling this method. Please call
     * RequestShutdown() to ask the user to decide shutdown or not.
     */
    Q_SCRIPTABLE void Shutdown();

    Q_SCRIPTABLE void Suspend();

    Q_SCRIPTABLE void Hibernate();

    /**
     * @since unity8
     */
    Q_SCRIPTABLE void HybridSleep();

    /**
     * Issue a logout request.
     *
     * This method emits the LogoutRequested signal to the shell with a boolean
     * which indicates if there's any inhibitors. The shell should receive
     * this signal and display a dialog to ask the user to confirm the logout
     * action. If confirmed, shell can call Logout() method to logout.
     */
    Q_SCRIPTABLE void RequestLogout();

    /**
     * Issue a reboot request.
     *
     * This method emits the RebootRequested signal to the shell with a boolean
     * which indicates if there's any inhibitors. The shell should receive
     * this signal and display a dialog to ask the user to confirm the reboot
     * action. If confirmed, shell can call Reboot() method to reboot.
     */
    Q_SCRIPTABLE void RequestReboot();

    /**
     * Issue a shutdown request.
     *
     * This method emits the ShutdownRequested signal to the shell with a
     * boolean which indicates if there's any inhibitors.
     * The shell should receive
     * this signal and display a dialog to ask the user to confirm the reboot
     * action. If confirmed, shell can call Shutdown() method to shutdown.
     */
    Q_SCRIPTABLE void RequestShutdown();

    /**
     * Issue an EndSession request.
     *
     * This method calls the EndSession() Upstart DBus method on the
     * current DBus session bus.
     */
    Q_SCRIPTABLE void EndSession();

    Q_SCRIPTABLE bool CanHibernate() const;

    Q_SCRIPTABLE bool CanSuspend() const;

    /**
     * @since unity8
     */
    Q_SCRIPTABLE bool CanHybridSleep() const;

    /**
     * @since unity8
     */
    Q_SCRIPTABLE bool CanReboot() const;

    Q_SCRIPTABLE bool CanShutdown() const;

    Q_SCRIPTABLE bool CanLock() const;

    Q_SCRIPTABLE QString UserName() const;

    Q_SCRIPTABLE QString RealName() const;

    Q_SCRIPTABLE QString HostName() const;

    Q_SCRIPTABLE void PromptLock();

    Q_SCRIPTABLE void Lock();
};

class DBusGnomeSessionManagerWrapper : public UnityDBusObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.gnome.SessionManager.EndSessionDialog")

public:
    DBusGnomeSessionManagerWrapper();
    ~DBusGnomeSessionManagerWrapper() = default;

public Q_SLOTS:
    Q_SCRIPTABLE void Open(const unsigned int type, const unsigned int arg_1, const unsigned int max_wait, const QList<QDBusObjectPath> &inhibitors);

private:
    void performAsyncCall(const QString &method);
};
#endif // DBUSUNITYSESSIONSERVICE_H
