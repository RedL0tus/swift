/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <boost/shared_ptr.hpp>

#include "Swiften/Elements/Stanza.h"
#include "Swiften/Elements/Payload.h"
#include "Swiften/Elements/Message.h"

using namespace Swift;

class StanzaTest : public CppUnit::TestFixture
{
		CPPUNIT_TEST_SUITE(StanzaTest);
		CPPUNIT_TEST(testConstructor_Copy);
		CPPUNIT_TEST(testGetPayload);
		CPPUNIT_TEST(testGetPayloads);
		CPPUNIT_TEST(testGetPayload_NoSuchPayload);
		CPPUNIT_TEST(testDestructor);
		CPPUNIT_TEST(testDestructor_Copy);
		CPPUNIT_TEST(testUpdatePayload_ExistingPayload);
		CPPUNIT_TEST(testUpdatePayload_NewPayload);
		CPPUNIT_TEST(testGetPayloadOfSameType);
		CPPUNIT_TEST(testGetPayloadOfSameType_NoSuchPayload);
		CPPUNIT_TEST_SUITE_END();

	public:
		class MyPayload1 : public Payload {
			public:
				MyPayload1() {}
		};

		class MyPayload2 : public Payload {
			public:
				MyPayload2(const String& s = "") : text_(s) {}

				String text_;
		};

		class MyPayload3 : public Payload {
			public:
				MyPayload3() {}
		};

		class DestroyingPayload : public Payload {
			public:
				DestroyingPayload(bool* alive) : alive_(alive) {
				}

				~DestroyingPayload() {
					(*alive_) = false;
				}
			
			private:
				bool* alive_;
		};

		StanzaTest() {}

		void testConstructor_Copy() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload2>(new MyPayload2()));
			Message copy(m);

			CPPUNIT_ASSERT(copy.getPayload<MyPayload1>());
			CPPUNIT_ASSERT(copy.getPayload<MyPayload2>());
		}

		void testDestructor() {
			bool payloadAlive = true;
			{
				Message m;
				m.addPayload(boost::shared_ptr<DestroyingPayload>(new DestroyingPayload(&payloadAlive)));
			}

			CPPUNIT_ASSERT(!payloadAlive);
		}

		void testDestructor_Copy() {
			bool payloadAlive = true;
			Message* m1 = new Message();
			m1->addPayload(boost::shared_ptr<DestroyingPayload>(new DestroyingPayload(&payloadAlive)));
			Message* m2 = new Message(*m1);

			delete m1;
			CPPUNIT_ASSERT(payloadAlive);

			delete m2;
			CPPUNIT_ASSERT(!payloadAlive);
		}

		void testGetPayload() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload2>(new MyPayload2()));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT(p);
		}

		void testGetPayload_NoSuchPayload() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT(!p);
		}

		void testGetPayloads() {
			Message m;
			boost::shared_ptr<MyPayload2> payload1(new MyPayload2());
			boost::shared_ptr<MyPayload2> payload2(new MyPayload2());
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(payload1);
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));
			m.addPayload(payload2);

			CPPUNIT_ASSERT_EQUAL((size_t)2, m.getPayloads<MyPayload2>().size());
			CPPUNIT_ASSERT_EQUAL(payload1, m.getPayloads<MyPayload2>()[0]);
			CPPUNIT_ASSERT_EQUAL(payload2, m.getPayloads<MyPayload2>()[1]);
		}


		void testUpdatePayload_ExistingPayload() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload2>(new MyPayload2("foo")));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			m.updatePayload(boost::shared_ptr<MyPayload2>(new MyPayload2("bar")));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), m.getPayloads().size());
			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT_EQUAL(String("bar"), p->text_);
		}

		void testUpdatePayload_NewPayload() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			m.updatePayload(boost::shared_ptr<MyPayload2>(new MyPayload2("bar")));

			CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), m.getPayloads().size());
			boost::shared_ptr<MyPayload2> p(m.getPayload<MyPayload2>());
			CPPUNIT_ASSERT_EQUAL(String("bar"), p->text_);
		}

		void testGetPayloadOfSameType() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload2>(new MyPayload2("foo")));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			boost::shared_ptr<MyPayload2> payload(boost::dynamic_pointer_cast<MyPayload2>(m.getPayloadOfSameType(boost::shared_ptr<MyPayload2>(new MyPayload2("bar")))));
			CPPUNIT_ASSERT(payload);
			CPPUNIT_ASSERT_EQUAL(String("foo"), payload->text_);
		}

		void testGetPayloadOfSameType_NoSuchPayload() {
			Message m;
			m.addPayload(boost::shared_ptr<MyPayload1>(new MyPayload1()));
			m.addPayload(boost::shared_ptr<MyPayload3>(new MyPayload3()));

			CPPUNIT_ASSERT(!m.getPayloadOfSameType(boost::shared_ptr<MyPayload2>(new MyPayload2("bar"))));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION(StanzaTest);
