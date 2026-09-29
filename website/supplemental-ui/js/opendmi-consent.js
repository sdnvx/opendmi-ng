/*
 * OpenDMI: Cross-platform DMI/SMBIOS framework
 * Copyright (c) 2025-2026, The OpenDMI contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Consent to the analytics of the site.
 *
 * Google Analytics sets cookies, which are set only once the visitor has
 * accepted them: until then its script is not loaded at all. The choice is
 * asked for by a banner at the foot of the page, which is shown until the
 * visitor makes it, is remembered by the browser, and is changed through the
 * link the footer of every page has.
 */
;(function () {
    'use strict'

    var script  = document.currentScript
    var id      = script && script.dataset.analyticsId
    var privacy = script && script.dataset.privacyUrl

    if (!id)
        return

    var storageKey = 'opendmi-analytics-consent'
    var banner     = null
    var loaded     = false

    function readChoice()
    {
        try {
            return window.localStorage.getItem(storageKey)
        } catch (error) {
            return null
        }
    }

    function writeChoice(choice)
    {
        try {
            window.localStorage.setItem(storageKey, choice)
        } catch (error) {
            // Choice is kept for the page alone when the browser keeps
            // nothing, and is asked for again on the next one
        }
    }

    function loadAnalytics()
    {
        window['ga-disable-' + id] = false

        if (loaded)
            return

        loaded = true

        window.dataLayer = window.dataLayer || []
        window.gtag = function () { window.dataLayer.push(arguments) }
        window.gtag('js', new Date())
        window.gtag('config', id)

        var tag = document.createElement('script')

        tag.async = true
        tag.src   = 'https://www.googletagmanager.com/gtag/js?id=' + encodeURIComponent(id)

        document.head.appendChild(tag)
    }

    // Analytics loaded on the page before the consent has been withdrawn is
    // stopped, and the cookies it has set are removed
    function stopAnalytics()
    {
        window['ga-disable-' + id] = true

        var domain = window.location.hostname.replace(/^www\./, '')

        document.cookie.split(';').forEach(function (cookie) {
            var name = cookie.split('=')[0].trim()

            if (!/^_ga/.test(name))
                return

            ;['', '; domain=' + domain, '; domain=.' + domain].forEach(function (scope) {
                document.cookie = name + '=; expires=Thu, 01 Jan 1970 00:00:00 GMT; path=/' + scope
            })
        })
    }

    function choose(choice)
    {
        writeChoice(choice)

        if (choice === 'granted')
            loadAnalytics()
        else
            stopAnalytics()

        hideBanner()
    }

    function hideBanner()
    {
        if (banner === null)
            return

        banner.remove()
        banner = null
    }

    function showBanner()
    {
        if (banner !== null)
            return

        banner = document.createElement('div')
        banner.className = 'opendmi-consent'
        banner.setAttribute('role', 'dialog')
        banner.setAttribute('aria-label', 'Cookie consent')
        banner.innerHTML =
            '<p class="opendmi-consent-text">' +
            'This site uses cookies of Google Analytics to learn how it is visited. ' +
            'They are set only if you accept them, and you can change your choice at ' +
            'any time through the link at the foot of every page.' +
            (privacy ? ' <a href="' + privacy + '">Learn more</a>' : '') +
            '</p>' +
            '<div class="opendmi-consent-actions">' +
            '<button type="button" class="opendmi-consent-accept">Accept</button>' +
            '<button type="button" class="opendmi-consent-decline">Decline</button>' +
            '</div>'

        banner.querySelector('.opendmi-consent-accept').addEventListener('click', function () {
            choose('granted')
        })

        banner.querySelector('.opendmi-consent-decline').addEventListener('click', function () {
            choose('denied')
        })

        document.body.appendChild(banner)
    }

    function start()
    {
        var choice = readChoice()

        if (choice === 'granted')
            loadAnalytics()
        else if (choice !== 'denied')
            showBanner()

        var link = document.querySelector('.opendmi-consent-settings')

        if (link !== null) {
            link.addEventListener('click', function (event) {
                event.preventDefault()
                showBanner()
            })
        }
    }

    if (document.readyState === 'loading')
        document.addEventListener('DOMContentLoaded', start)
    else
        start()
})()
