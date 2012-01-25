#!/usr/bin/env python

# Dependencies to run this:
#  - xmlto in $PATH

# FIXME:
#  - correctly handle all exceptions
#  - copy dtd files where they should be
#  - new structure for website:
#    specs.fd.o/index.html -- general index
#    specs.fd.o/desktop-entry/index.html -- index of all versions of desktop entry, with all formats
#    specs.fd.o/desktop-entry/1.0/desktop-entry-spec.xml -- docbook version of the spec 1.0
#    specs.fd.o/desktop-entry/1.0/index.html -- one-page html version of the spec 1.0
#    specs.fd.o/desktop-entry/1.0/split/ -- multiple-page html version of the spec 1.0
#    specs.fd.o/desktop-entry/latest/ -- link to directory containing latest version of the spec

import os
import sys

import errno

import StringIO
import hashlib
import shutil
import subprocess
import urllib
import urllib2
import urlparse

DEVELOPMENT = False

CVSWEB = 'http://cvs.freedesktop.org'
GITWEB = 'http://cgit.freedesktop.org'
HASH = 'md5'


def safe_mkdir(dir):
    if not dir:
        return

    try:
        os.mkdir(dir)
    except OSError, e:
        if e.errno != errno.EEXIST:
            raise e


def get_hash_from_fd(fd, algo = HASH, read_blocks = 1024):
    if algo not in [ 'md5' ]:
        raise Exception('Internal error: hash algorithm \'%s\' not planned in code.' % algo)

    hash = hashlib.new(algo)
    while True:
        data = fd.read(read_blocks)
        if not data:
            break
        hash.update(data)
    return hash.digest()


def get_hash_from_url(url, algo = HASH):
    fd = urllib2.urlopen(url, None)
    digest = get_hash_from_fd(fd, algo)
    fd.close()
    return digest


def get_hash_from_path(path, algo = HASH):
    fd = open(path, 'rb')
    digest = get_hash_from_fd(fd, algo, read_blocks = 32768)
    fd.close()
    return digest


def get_hash_from_data(data, algo = HASH):
    fd = StringIO.StringIO(data)
    digest = get_hash_from_fd(fd, algo, read_blocks = 32768)
    fd.close()
    return digest


class VcsObject:
    def __init__(self, vcs, repo, file, revision = None):
        self.vcs = vcs
        self.repo = repo
        self.file = file
        self.revision = revision
        self.data = None

    def get_url(self):
        query = {}
        if self.vcs == 'git':
            baseurl = GITWEB
            path = '/'.join((self.repo, 'plain', self.file))
            if self.revision:
                query['id'] = self.revision
        elif self.vcs == 'cvs':
            baseurl = CVSWEB
            path = self.file
            if self.revision:
                query['rev'] = self.revision
        else:
            raise Exception('Unknown VCS: %s' % self.vcs)

        (scheme, netloc, basepath) = urlparse.urlsplit(baseurl)[0:3]
        full_path = '/'.join((basepath, path))

        query_str = urllib.urlencode(query)
        return urlparse.urlunsplit((scheme, netloc, full_path, query_str, ''))

    def fetch(self):
        if self.data:
            return

        url = self.get_url()
        fd = urllib2.urlopen(url, None)
        self.data = fd.read()
        fd.close()

    def get_hash(self):
        self.fetch()
        return get_hash_from_data(self.data)


class SpecObject():
    def __init__(self, vcs, spec_dir, version):
        self.vcs = vcs
        self.spec_dir = spec_dir
        self.version = version

        basename = os.path.basename(self.vcs.file)
        (self.basename_no_ext, self.ext) = os.path.splitext(basename)

        self.filename = '%s-%s%s' % (self.basename_no_ext, self.version, self.ext)

        if self.ext not in ['.xml', '.sgml', '.txt', '.dtd']:
            raise Exception('Format \'%s\' not supported for %s' % (self.ext, self.vcs.get_url()))

        self.downloaded = False
        self.one_chunk = False
        self.multiple_chunks = False

    def download(self):
        safe_mkdir(self.spec_dir)
        path = os.path.join(self.spec_dir, self.filename)

        if os.path.exists(path):
            current_hash = get_hash_from_path(path)
            vcs_hash = self.vcs.get_hash()
            if current_hash == vcs_hash:
                return

        self.vcs.fetch()
        fd = open(path, 'wb')
        fd.write(self.vcs.data)
        fd.close()

        self.downloaded = True

    def htmlize(self, force = False):
        if not self.downloaded and not force:
            return

        path = os.path.join(self.spec_dir, self.filename)
        (path_no_ext, ext) = os.path.splitext(path)

        if self.ext == '.xml':
            # One-chunk HTML
            html_path = '%s%s' % (path_no_ext, '.html')
            if os.path.exists(html_path):
                os.unlink(html_path)

            retcode = subprocess.call(['xmlto', '-o', self.spec_dir, 'html-nochunks', path])

            if retcode != 0:
                raise Exception('Cannot convert \'%s\' to HTML.' % path)
            self.one_chunk = True

            # Multiple chunks
            html_dir = os.path.join(self.spec_dir, self.version)
            if os.path.exists(html_dir):
                shutil.rmtree(html_dir)
            safe_mkdir(html_dir)

            retcode = subprocess.call(['xmlto', '-o', html_dir, 'html', path])

            if retcode != 0:
                raise Exception('Cannot convert \'%s\' to multiple-chunks HTML.' % path)
            self.multiple_chunks = True

    def latestize(self):
        filename_latest = '%s-latest%s' % (self.basename_no_ext, self.ext)

        path_latest = os.path.join(self.spec_dir, filename_latest)
        if os.path.exists(path_latest):
            os.unlink(path_latest)
        os.symlink(self.filename, path_latest)

        if self.ext == '.xml':
            # One-chunk HTML
            html_path_latest = os.path.join(self.spec_dir, '%s%s' % (self.basename_no_ext, '.html'))
            if os.path.exists(html_path_latest):
                os.unlink(html_path_latest)

            (filename_no_ext, ext) = os.path.splitext(self.filename)
            html_filename = '%s%s' % (filename_no_ext, '.html')
            html_path = os.path.join(self.spec_dir, html_filename)
            if os.path.exists(html_path):
                os.symlink(html_filename, html_path_latest)

            # Multiple chunks
            html_dir_latest = os.path.join(self.spec_dir, 'latest')
            if os.path.exists(html_dir_latest):
                os.unlink(html_dir_latest)

            html_dir = os.path.join(self.spec_dir, self.version)
            if os.path.exists(html_dir):
                os.symlink(self.version, html_dir_latest)


SCRIPT = VcsObject('git', 'xdg/xdg-specs', 'web-export/update.py')
SPECS_INDEX = VcsObject('git', 'xdg/xdg-specs', 'web-export/specs.idx')


def is_up_to_date():
    current_hash = get_hash_from_path(__file__)
    vcs_hash = SCRIPT.get_hash()

    return current_hash == vcs_hash


if not DEVELOPMENT:
    if not is_up_to_date():
        print >>sys.stderr, 'Script is not up-to-date, please download %s' % SCRIPT.get_url()
        sys.exit(1)

    SPECS_INDEX.fetch()
    lines = SPECS_INDEX.data.split('\n')
else:
    lines = open('specs.idx').readlines()


latests = []

for line in lines:
    line = line.strip()
    if not line or line.startswith('#'):
        continue

    (data, revision, version, path) = line.split()
    if data.startswith("git:"):
        git_data = data.split(":")
        vcs = VcsObject('git', git_data[1], git_data[2], revision)
    else:
        vcs = VcsObject('cvs', None, data, revision)

    spec = SpecObject(vcs, path, version)

    spec.download()
    spec.htmlize()

    # Create latest links if it's the first time we see this spec
    if (spec.spec_dir, spec.basename_no_ext) not in latests:
        latests.append((spec.spec_dir, spec.basename_no_ext))
        spec.latestize()
